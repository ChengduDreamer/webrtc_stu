#pragma once

#include <functional>
#include <NlohmannJson/json.hpp>
#include <api/media_stream_interface.h>
#include <api/peer_connection_interface.h>
#include "rtc_base/thread.h"


#include "api/async_dns_resolver.h"
#include "api/task_queue/pending_task_safety_flag.h"
#include "rtc_base/net_helpers.h"
#include "rtc_base/physical_socket_server.h"
#include "rtc_base/third_party/sigslot/sigslot.h"
#include "rtc_base/ref_count.h"

class QWidget;

namespace yk {


	//struct PeerConnectionClientObserver {
	//	virtual void OnSignedIn() = 0;  // Called when we're logged on.
	//	virtual void OnDisconnected() = 0;
	//	virtual void OnPeerConnected(int id, const std::string& name) = 0;
	//	virtual void OnPeerDisconnected(int peer_id) = 0;
	//	virtual void OnMessageFromPeer(int peer_id, const std::string& message) = 0;
	//	virtual void OnMessageSent(int err) = 0;
	//	virtual void OnServerConnectionFailure() = 0;

	//protected:
	//	virtual ~PeerConnectionClientObserver() {}
	//};


	class RtcManager
					: public webrtc::PeerConnectionObserver
						//public PeerConnectionClientObserver
					   , public webrtc::CreateSessionDescriptionObserver
	{
	public:
		RtcManager();
		~RtcManager();

		bool InitializePeerConnection();
		bool CreatePeerConnection();

		void AddTracks();

		void SetLocalRenderWidget(QWidget* w);

		void CreateOffer();

		void CreateAnswer();

		void OnMessageFromPeer(const nlohmann::json jsobj);


		QWidget* loacl_render_widget_ = nullptr;

		std::unique_ptr<rtc::Thread> signaling_thread_ = nullptr;
		rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_ = nullptr;
		rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory_ = nullptr;



		//
		// PeerConnectionObserver implementation.
		//

		void OnSignalingChange(
			webrtc::PeerConnectionInterface::SignalingState new_state) override {}
		void OnAddTrack(
			rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
			const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>&
			streams) override;
		void OnRemoveTrack(
			rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver) override;
		void OnDataChannel(
			rtc::scoped_refptr<webrtc::DataChannelInterface> channel) override {}
		void OnRenegotiationNeeded() override {}

		void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override;

		void OnIceGatheringChange(
			webrtc::PeerConnectionInterface::IceGatheringState new_state) override {}
		void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
		void OnIceConnectionReceivingChange(bool receiving) override {}


		// CreateSessionDescriptionObserver implementation.
		void OnSuccess(webrtc::SessionDescriptionInterface* desc) override;
		void OnFailure(webrtc::RTCError error) override;


		// RefCountInterface imple //不知道为什么官方demo中 就没实现这两个虚函数
		mutable webrtc::webrtc_impl::RefCounter ref_count_{ 0 };
		bool HasOneRef() const 
		{ 
			return ref_count_.HasOneRef(); 
		}
		void AddRef() const override 
		{ 
			ref_count_.IncRef(); 
		}
		rtc::RefCountReleaseStatus Release() const override {
			const auto status = ref_count_.DecRef();
			if (status == rtc::RefCountReleaseStatus::kDroppedLastRef) {
				//delete this; // to do  这里先注释掉 不然会析构此对象
			}
			return status;
		}


		

		                                                        // 
		using OnCreatedSDPMsgCallbackFunc = std::function<void(std::string content, std::string type)>;

		OnCreatedSDPMsgCallbackFunc on_created_sdp_msg_callback_ = nullptr;

		void SetOnCreatedSDPMsgCallback(OnCreatedSDPMsgCallbackFunc&& cbk) {
			on_created_sdp_msg_callback_ = std::move(cbk);
		}


		using OnIceCandidateCallbackFunc = std::function<void(nlohmann::json ice_jsobj)>;

		OnIceCandidateCallbackFunc on_ice_candidate_callback_ = nullptr;

		void SetOnIceCandidateCallback(OnIceCandidateCallbackFunc&& cbk) {
			on_ice_candidate_callback_ = std::move(cbk);
		}


		void OnRecvOfferFromPeer();
	};

}