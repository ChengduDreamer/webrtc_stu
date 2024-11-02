#pragma once

#include <api/media_stream_interface.h>
#include <api/peer_connection_interface.h>

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
					   //public webrtc::CreateSessionDescriptionObserver
	{
	public:
		RtcManager();
		~RtcManager();

		bool InitializePeerConnection();
		bool CreatePeerConnection();

		void AddTracks();

		void SetLocalRenderWidget(QWidget* w);

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
		void OnIceConnectionChange(
			webrtc::PeerConnectionInterface::IceConnectionState new_state) override {}
		void OnIceGatheringChange(
			webrtc::PeerConnectionInterface::IceGatheringState new_state) override {}
		void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
		void OnIceConnectionReceivingChange(bool receiving) override {}

	};

}