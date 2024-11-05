#include "signals_server.h"
#include <iostream>
#include <NlohmannJson/json.hpp>
#include "web_socket_server.h"
#include "public/signals_def.h"

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
		std::cout << "SignalsServer::OnRecvMsg : " << msg << std::endl;
		nlohmann::json jsobj = nlohmann::json::parse(msg);
		const std::string op_type = jsobj["operation_type"].get<std::string>();

		if (kSignalsMsgType_Hello == op_type) {
			const std::string client_id = jsobj["client_id"].get<std::string>();
			client_id_set_.insert(client_id);
		}
		else if (kSignalsMsgType_Call == op_type) {
			const std::string caller_client_id = jsobj["caller_client_id"].get<std::string>();
			const std::string callee_client_id = jsobj["callee_client_id"].get<std::string>();




		}
	}
}