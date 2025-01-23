#include "signals_client.h"
#include <NlohmannJson/json.hpp>
#include "web_socket_client.h"
#include "settings.h"
#include "context.h"
#include "yk_logger.h"
#include "public/signals_def.h"
namespace yk {

	SignalsClient::SignalsClient(const std::shared_ptr<Context>& ctx_ptr): ctx_ptr_(ctx_ptr) {
		Init();
	}

	SignalsClient::~SignalsClient() {
	
	}

	void SignalsClient::Init() {
		auto settings = Settings::GetInstance();
		ws_ptr_ = WebSocketClient::Make("client_" + settings->client_id);

		
		ws_ptr_->on_message.connect([=](const std::string recv_msg) {
			RecvMsg(recv_msg);
		});


		if (!ws_ptr_->Connect(*ctx_ptr_->GetIoPool().get(), settings->signalling_host, settings->signalling_port, "/")) {
			YK_LOGE("SignalsClient ws connect error.");
		}
		// to do ÐÄÌø


	}
	
	void SignalsClient::SayHello() {

		task_worker_.AsyncTask(
			[=]() {
				if (!ws_ptr_) {
					return;
				}
				auto settings = Settings::GetInstance();
				nlohmann::json msg_json;
				msg_json["client_id"] = settings->client_id;
				msg_json["operation_type"] = kSignalsMsgType_Hello;
				std::string msg_str = msg_json.dump();
				ws_ptr_->AsyncSendBin(msg_str);
			},
			[]() {
			}
		);
	}

	void SignalsClient::Call(const std::string& remote_id) {

		task_worker_.AsyncTask(
			[=]() {
				auto settings = Settings::GetInstance();
				nlohmann::json msg_json;
				msg_json["caller_client_id"] = settings->client_id;
				msg_json["callee_client_id"] = remote_id;
				msg_json["operation_type"] = kSignalsMsgType_Call;
				std::string msg_str = msg_json.dump();
				ws_ptr_->AsyncSendBin(msg_str);
			},
			[]() {
			}
		);	
	}

	void SignalsClient::RecvMsg(const std::string msg) {
		YK_LOGI("RecvMsg : {}", msg);
		task_worker_.AsyncTask(
			[=]() {
				HandleMsg(msg);
			},
			[]() {
			}
		);
	}

	void SignalsClient::HandleMsg(const std::string msg) {

		nlohmann::json jsobj = nlohmann::json::parse(msg);
		const std::string op_type = jsobj["operation_type"].get<std::string>();

		if (kSignalsMsgType_CreatedRoom == op_type) {
			HandleCreatedRoomMsg(jsobj);
		}
		else if (kSignalsMsgType_Message == op_type) {
			HandleRecvSDPMsg(jsobj);
		}
	}


	void SignalsClient::HandleCreatedRoomMsg(const nlohmann::json& jsobj) {
		room_id_ = jsobj["room_id"].get<std::string>();


		YK_LOGI("HandleCreatedRoomMsg the room_id : {}", room_id_);


		if (on_created_room_msg_callback_) {
			on_created_room_msg_callback_();
		}
	}


	void SignalsClient::SendSDPMsg(const std::string& sdp, const std::string& sdp_type) {


		YK_LOGI("SendSDPMsg room_id : {}", room_id_);


		task_worker_.AsyncTask(
			[=]() {
				auto settings = Settings::GetInstance();
				nlohmann::json jsobj;
				jsobj["room_id"] = room_id_;
				jsobj["client_id"] = settings->client_id;
				jsobj["operation_type"] = kSignalsMsgType_Message;
				jsobj["content"] = sdp;
				jsobj["sdp_type"] = sdp_type;
				std::string msg_str = jsobj.dump();
				ws_ptr_->AsyncSendBin(msg_str);
			},
			[]() {
			}
		);
	}


	void SignalsClient::SendIceCandidateMsg(nlohmann::json ice_jsobj) {
		task_worker_.AsyncTask(
			[=]() mutable {
				auto settings = Settings::GetInstance();
				ice_jsobj["room_id"] = room_id_;
				ice_jsobj["client_id"] = settings->client_id;
				ice_jsobj["operation_type"] = kSignalsMsgType_Message;
				std::string msg_str = ice_jsobj.dump();
				ws_ptr_->AsyncSendBin(msg_str);
			},
			[]() {
			}
		);
	}


	void SignalsClient::HandleRecvSDPMsg(const nlohmann::json& jsobj) {
		
		if (on_recv_sdp_msg_callback_) {
		
			on_recv_sdp_msg_callback_(jsobj);
		}
		
	}
}