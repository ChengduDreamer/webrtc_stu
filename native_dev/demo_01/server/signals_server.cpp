#include "signals_server.h"
#include <iostream>
#include <NlohmannJson/json.hpp>
#include "web_socket_server.h"
#include "public/signals_def.h"
#include "room_manager.h"
#include "common/yk_logger.h"


namespace yk {

	SignalsServer::SignalsServer() {
		Init();
	}

	SignalsServer::~SignalsServer() {
		
	}

	void SignalsServer::Init() {
		websocket_server_ptr_ = WebSocketServer::Make();
		websocket_server_ptr_->SetOnRecvMsgCallback(std::move([=](std::string data) {
			OnRecvMsg(data);
		}));
	}

	void SignalsServer::OnRecvMsg(const std::string& msg) {
		task_worker_.AsyncTask(
			[=]() {
				HandleMsg(msg);
			},
			[]() {
			
			}
		);
	}


	void SignalsServer::HandleMsg(const std::string msg) {
		std::cout << "SignalsServer::OnRecvMsg : " << msg << std::endl;
		nlohmann::json jsobj = nlohmann::json::parse(msg);
		const std::string op_type = jsobj["operation_type"].get<std::string>();

		if (kSignalsMsgType_Hello == op_type) {
			const std::string client_id = jsobj["client_id"].get<std::string>();
			client_id_set_.insert(client_id);
		}
		else if (kSignalsMsgType_Call == op_type) {
			HandleCallMsg(jsobj);
		}
		else if (kSignalsMsgType_Message == op_type) {
			HandleForwardMsg(jsobj);
		}
	}

	void SignalsServer::ShowAllClientId() {
		for (auto& id : client_id_set_) {
			std::cout << "client id : " << id << std::endl;
		}
	}

	void SignalsServer::HandleCallMsg(const nlohmann::json& jsobj) {

		nlohmann::json resp_obj;

		const std::string caller_client_id = jsobj["caller_client_id"].get<std::string>();
		const std::string callee_client_id = jsobj["callee_client_id"].get<std::string>();

		//判断被呼叫端是否已经连上信令服务
		if (client_id_set_.find(callee_client_id) == client_id_set_.end()) {
			resp_obj["operation_type"] = kSignalsMsgType_OtherNotOnline;
			return;
		}
		std::string room_id = RoomManager::Instance()->CreateRoom(caller_client_id, callee_client_id);
		resp_obj["operation_type"] = kSignalsMsgType_CreatedRoom;
		resp_obj["room_id"] = room_id;

		YK_LOGI("SignalsServer HandleCallMsg SendMsg to {}, msg is {}", caller_client_id, resp_obj.dump());

		websocket_server_ptr_->SendMsg(caller_client_id, resp_obj.dump());

		//websocket_server_ptr_->SendMsg(callee_client_id, resp_obj.dump());
	}


	void SignalsServer::HandleForwardMsg(const nlohmann::json& jsobj) {
		
		try {
			std::string room_id = jsobj["room_id"].get<std::string>();

			YK_LOGI("HandleForwardMsg room_id: {}", room_id);

			if (room_id.empty()) {

				YK_LOGI("HandleForwardMsg room_id is empty");
				return;
			}

			std::string client_id = jsobj["client_id"].get<std::string>();

			auto room = RoomManager::Instance()->GetRoom(room_id);

			if (room->caller_id_ == client_id) {
				websocket_server_ptr_->SendMsg(room->callee_id_, jsobj.dump());
			}
			else if (room->callee_id_ == client_id) {
				websocket_server_ptr_->SendMsg(room->caller_id_, jsobj.dump());
			}
		}
		catch (std::exception& e) {
			YK_LOGE("excepiton msg : {}", e.what());
		}
	}
}