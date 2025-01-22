#include "rtc_manager.h"

#include <stddef.h>
#include <stdint.h>

#include <memory>
#include <utility>
#include <vector>
#include <iostream>

#include <dshow.h>

#include "absl/memory/memory.h"
#include "absl/types/optional.h"
#include "api/audio/audio_mixer.h"
#include "api/audio_codecs/audio_decoder_factory.h"
#include "api/audio_codecs/audio_encoder_factory.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/audio_options.h"
#include "api/create_peerconnection_factory.h"
#include "api/rtp_sender_interface.h"
#include "api/video_codecs/video_decoder_factory.h"
#include "api/video_codecs/video_decoder_factory_template.h"
#include "api/video_codecs/video_decoder_factory_template_dav1d_adapter.h"
#include "api/video_codecs/video_decoder_factory_template_libvpx_vp8_adapter.h"
#include "api/video_codecs/video_decoder_factory_template_libvpx_vp9_adapter.h"
#include "api/video_codecs/video_decoder_factory_template_open_h264_adapter.h"
#include "api/video_codecs/video_encoder_factory.h"
#include "api/video_codecs/video_encoder_factory_template.h"
#include "api/video_codecs/video_encoder_factory_template_libaom_av1_adapter.h"
#include "api/video_codecs/video_encoder_factory_template_libvpx_vp8_adapter.h"
#include "api/video_codecs/video_encoder_factory_template_libvpx_vp9_adapter.h"
#include "api/video_codecs/video_encoder_factory_template_open_h264_adapter.h"
#include "examples/peerconnection/client/defaults.h"
#include "modules/audio_device/include/audio_device.h"
#include "modules/audio_processing/include/audio_processing.h"
#include "modules/video_capture/video_capture.h"
#include "modules/video_capture/video_capture_factory.h"
#include "p2p/base/port_allocator.h"
#include "pc/video_track_source.h"
#include "rtc_base/checks.h"
#include "rtc_base/logging.h"
#include "rtc_base/rtc_certificate_generator.h"
//#include "rtc_base/strings/json.h"
#include "vcm_capturer.h"

#include "ui/local_render_widget.h"

#include "common/yk_logger.h"

#pragma comment(lib, "strmiids.lib")

namespace yk {

    class DummySetSessionDescriptionObserver
        : public webrtc::SetSessionDescriptionObserver {
    public:
        static rtc::scoped_refptr<DummySetSessionDescriptionObserver> Create() {
            return rtc::make_ref_counted<DummySetSessionDescriptionObserver>();
        }
        virtual void OnSuccess() { RTC_LOG(LS_INFO) << __FUNCTION__; }
        virtual void OnFailure(webrtc::RTCError error) {
            RTC_LOG(LS_INFO) << __FUNCTION__ << " " << ToString(error.type()) << ": "
                << error.message();
        }
    };


    class CapturerTrackSource : public webrtc::VideoTrackSource {
    public:
        static rtc::scoped_refptr<CapturerTrackSource> Create() {
            const size_t kWidth = 640;
            const size_t kHeight = 480;
            const size_t kFps = 30;
            std::unique_ptr<webrtc::test::VcmCapturer> capturer;
            std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> info(
                webrtc::VideoCaptureFactory::CreateDeviceInfo());
            if (!info) {
                return nullptr;
            }
            int num_devices = info->NumberOfDevices();
            for (int i = 0; i < num_devices; ++i) {
                capturer = absl::WrapUnique(
                    webrtc::test::VcmCapturer::Create(kWidth, kHeight, kFps, i));
                if (capturer) {
                    return rtc::make_ref_counted<CapturerTrackSource>(std::move(capturer));
                }
            }

            return nullptr;
        }

    protected:
        explicit CapturerTrackSource(
            std::unique_ptr<webrtc::test::VcmCapturer> capturer)
            : VideoTrackSource(/*remote=*/false), capturer_(std::move(capturer)) {}

    private:
        rtc::VideoSourceInterface<webrtc::VideoFrame>* source() override {
            return capturer_.get();
        }
        std::unique_ptr<webrtc::test::VcmCapturer> capturer_;
    };



	RtcManager::RtcManager() {

	}

	RtcManager::~RtcManager() {

	}

	bool RtcManager::InitializePeerConnection() {

        if (!signaling_thread_.get()) {
            signaling_thread_ = rtc::Thread::CreateWithSocketServer();
            signaling_thread_->Start();
        }
		
        peer_connection_factory_ = webrtc::CreatePeerConnectionFactory(
            nullptr /* network_thread */, nullptr /* worker_thread */,
            signaling_thread_.get(), nullptr /* default_adm */,
            webrtc::CreateBuiltinAudioEncoderFactory(),
            webrtc::CreateBuiltinAudioDecoderFactory(),
            std::make_unique<webrtc::VideoEncoderFactoryTemplate<
            webrtc::LibvpxVp8EncoderTemplateAdapter,
            webrtc::LibvpxVp9EncoderTemplateAdapter,
            webrtc::OpenH264EncoderTemplateAdapter,
            webrtc::LibaomAv1EncoderTemplateAdapter>>(),
            std::make_unique<webrtc::VideoDecoderFactoryTemplate<
            webrtc::LibvpxVp8DecoderTemplateAdapter,
            webrtc::LibvpxVp9DecoderTemplateAdapter,
            webrtc::OpenH264DecoderTemplateAdapter,
            webrtc::Dav1dDecoderTemplateAdapter>>(),
            nullptr /* audio_mixer */, nullptr /* audio_processing */);

        if (!peer_connection_factory_) {
            std::cout << "peer_connection_factory_ init error" << std::endl;
            return false;
        }


		return true;
	}

    bool RtcManager::CreatePeerConnection() {
    
        webrtc::PeerConnectionInterface::RTCConfiguration config;
        config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
        webrtc::PeerConnectionInterface::IceServer server;
        server.uri = GetPeerConnectionString();
        config.servers.push_back(server);

        webrtc::PeerConnectionDependencies pc_dependencies(this);
        auto error_or_peer_connection =
            peer_connection_factory_->CreatePeerConnectionOrError(
                config, std::move(pc_dependencies));
        if (error_or_peer_connection.ok()) {
            peer_connection_ = std::move(error_or_peer_connection.value());
        }
        return peer_connection_ != nullptr;
    }

    void RtcManager::AddTracks() {
        if (!peer_connection_->GetSenders().empty()) {
            return;  // Already added tracks.
        }

        rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track(
            peer_connection_factory_->CreateAudioTrack(
                kAudioLabel,
                peer_connection_factory_->CreateAudioSource(cricket::AudioOptions())
                .get()));
        auto result_or_error = peer_connection_->AddTrack(audio_track, { kStreamId });
        if (!result_or_error.ok()) {
            RTC_LOG(LS_ERROR) << "Failed to add audio track to PeerConnection: "
                << result_or_error.error().message();
        }

        rtc::scoped_refptr<CapturerTrackSource> video_device = CapturerTrackSource::Create();
        if (video_device) {
            rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track_(
                peer_connection_factory_->CreateVideoTrack(video_device, kVideoLabel));


            //video_track_.get();

            //main_wnd_->StartLocalRenderer(video_track_.get());

            dynamic_cast<LocalRenderWidget*>(loacl_render_widget_)->StartLocalRenderer(video_track_.get());


            result_or_error = peer_connection_->AddTrack(video_track_, { kStreamId });
            if (!result_or_error.ok()) {
                RTC_LOG(LS_ERROR) << "Failed to add video track to PeerConnection: "
                    << result_or_error.error().message();
            }
        }
        else {
            RTC_LOG(LS_ERROR) << "OpenVideoCaptureDevice failed";
        }

        //main_wnd_->SwitchToStreamingUI();
    }

    //
    // PeerConnectionObserver implementation.
    //

    void RtcManager::OnAddTrack(
        rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
        const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>&
        streams) {
       /* RTC_LOG(LS_INFO) << __FUNCTION__ << " " << receiver->id();
        main_wnd_->QueueUIThreadCallback(NEW_TRACK_ADDED,
            receiver->track().release());*/
    }

    void RtcManager::OnRemoveTrack(
        rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver) {
       /* RTC_LOG(LS_INFO) << __FUNCTION__ << " " << receiver->id();
        main_wnd_->QueueUIThreadCallback(TRACK_REMOVED, receiver->track().release());*/
    }

    void RtcManager::OnIceCandidate(const webrtc::IceCandidateInterface* candidate) {

        YK_LOGI("OnIceCandidate : {}", candidate->sdp_mid());

        //RTC_LOG(LS_INFO) << __FUNCTION__ << " " << candidate->sdp_mline_index();
        //// For loopback test. To save some connecting delay.
        //if (loopback_) {
        //    if (!peer_connection_->AddIceCandidate(candidate)) {
        //        RTC_LOG(LS_WARNING) << "Failed to apply the received candidate";
        //    }
        //    return;
        //}

        //Json::Value jmessage;
        //jmessage[kCandidateSdpMidName] = candidate->sdp_mid();
        //jmessage[kCandidateSdpMlineIndexName] = candidate->sdp_mline_index();
        //std::string sdp;
        //if (!candidate->ToString(&sdp)) {
        //    RTC_LOG(LS_ERROR) << "Failed to serialize candidate";
        //    return;
        //}
        //jmessage[kCandidateSdpName] = sdp;

        //Json::StreamWriterBuilder factory;
        //SendMessage(Json::writeString(factory, jmessage));
    }

    void RtcManager::SetLocalRenderWidget(QWidget* w) {
        loacl_render_widget_ = w;
    }


    void RtcManager::CreateOffer() {
        
        peer_connection_->CreateOffer(this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
    }

    void RtcManager::CreateAnswer() {
        peer_connection_->CreateAnswer(this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
    }


    // 官方demo中，应该是没有区分具体的 类型，只要是message 就转发过来了，等后续对比下
    void RtcManager::OnRecvOfferFromPeer() {
        //absl::optional<webrtc::SdpType> type_maybe =
        //    webrtc::SdpTypeFromString(type_str);
        //if (!type_maybe) {
        //    RTC_LOG(LS_ERROR) << "Unknown SDP type: " << type_str;
        //    return;
        //}
        //webrtc::SdpType type = *type_maybe;
        //std::string sdp;
        //if (!rtc::GetStringFromJsonObject(jmessage, kSessionDescriptionSdpName,
        //    &sdp)) {
        //    RTC_LOG(LS_WARNING)
        //        << "Can't parse received session description message.";
        //    return;
        //}
        //webrtc::SdpParseError error;
        //std::unique_ptr<webrtc::SessionDescriptionInterface> session_description =
        //    webrtc::CreateSessionDescription(type, sdp, &error);
        //if (!session_description) {
        //    RTC_LOG(LS_WARNING)
        //        << "Can't parse received session description message. "
        //        "SdpParseError was: "
        //        << error.description;
        //    return;
        //}
        //RTC_LOG(LS_INFO) << " Received session description :" << message;
        //peer_connection_->SetRemoteDescription(
        //    DummySetSessionDescriptionObserver::Create().get(),
        //    session_description.release());
    }

    void RtcManager::OnSuccess(webrtc::SessionDescriptionInterface* desc) 
    {
        peer_connection_->SetLocalDescription(
            DummySetSessionDescriptionObserver::Create().get(), desc);

        std::string sdp;
        desc->ToString(&sdp);

        std::cout << "local sdp : " << sdp << std::endl;

        std::string sdp_type_str = webrtc::SdpTypeToString(desc->GetType());

        std::cout << "local sdp type : " << sdp_type_str << std::endl;  //offer
 
        if (on_created_sdp_msg_callback_) {
            on_created_sdp_msg_callback_(sdp, sdp_type_str);
        }


        // For loopback test. To save some connecting delay.
        //if (loopback_) {
        //    // Replace message type from "offer" to "answer"
        //    std::unique_ptr<webrtc::SessionDescriptionInterface> session_description =
        //        webrtc::CreateSessionDescription(webrtc::SdpType::kAnswer, sdp);
        //    peer_connection_->SetRemoteDescription(
        //        DummySetSessionDescriptionObserver::Create().get(),
        //        session_description.release());
        //    return;
        //}
        //
        //Json::Value jmessage;
        //jmessage[kSessionDescriptionTypeName] =
        //    webrtc::SdpTypeToString(desc->GetType());
        //jmessage[kSessionDescriptionSdpName] = sdp;
        //
        //Json::StreamWriterBuilder factory;
        //SendMessage(Json::writeString(factory, jmessage));



        


    }

    void RtcManager::OnFailure(webrtc::RTCError error) {
        RTC_LOG(LS_ERROR) << ToString(error.type()) << ": " << error.message();
    }


    void RtcManager::OnMessageFromPeer(const nlohmann::json jsobj) {
        // to do 这里的判断要改进
        if (jsobj.contains("sdp_type")) {
            std::string sdp_type = jsobj["sdp_type"].get<std::string>();
            std::string sdp = jsobj["content"].get<std::string>();
            std::string room_id = jsobj["room_id"].get<std::string>();

            absl::optional<webrtc::SdpType> type_maybe =
                webrtc::SdpTypeFromString(sdp_type);
            if (!type_maybe) {
                qDebug() << "Unknown SDP type: " << sdp_type;
                return;
            }
            webrtc::SdpType type = *type_maybe;

            webrtc::SdpParseError error;
            std::unique_ptr<webrtc::SessionDescriptionInterface> session_description = webrtc::CreateSessionDescription(type, sdp, &error);
            if (!session_description) {
                qDebug() << "Can't parse received session description message. SdpParseError was: " << error.description;
                return;
            }
            //qDebug() << " Received session description :" << message;
            peer_connection_->SetRemoteDescription(DummySetSessionDescriptionObserver::Create().get(), session_description.release());
            if (type == webrtc::SdpType::kOffer) {
                AddTracks();
                peer_connection_->CreateAnswer(this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
            }
        }
        else {

            YK_LOGI("OnMessageFromPeer : {}", jsobj.dump());


           /* std::string sdp_mid;
            int sdp_mlineindex = 0;
            std::string sdp;
            if (!rtc::GetStringFromJsonObject(jmessage, kCandidateSdpMidName, &sdp_mid) || !rtc::GetIntFromJsonObject(jmessage, kCandidateSdpMlineIndexName,&sdp_mlineindex) ||
                !rtc::GetStringFromJsonObject(jmessage, kCandidateSdpName, &sdp)) {
                RTC_LOG(LS_WARNING) << "Can't parse received message.";
                return;
            }
            webrtc::SdpParseError error;
            std::unique_ptr<webrtc::IceCandidateInterface> candidate(
                webrtc::CreateIceCandidate(sdp_mid, sdp_mlineindex, sdp, &error));
            if (!candidate.get()) {
                RTC_LOG(LS_WARNING) << "Can't parse received candidate message. "
                    "SdpParseError was: "
                    << error.description;
                return;
            }
            if (!peer_connection_->AddIceCandidate(candidate.get())) {
                RTC_LOG(LS_WARNING) << "Failed to apply the received candidate";
                return;
            }
            RTC_LOG(LS_INFO) << " Received candidate :" << message;*/
            
        
        
        
        }
    }
}