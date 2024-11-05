#pragma once
#include <memory>
#include <string>
#include <map>
#include <set>

namespace yk {

	class WebSocketServer;

	class SignalsServer {
	public:
		SignalsServer();
		~SignalsServer();
	private:
		void Init();
		void OnRecvMsg(const std::string& msg);
		std::shared_ptr<WebSocketServer> websocket_server_ptr_ = nullptr;

		
		std::set<std::string> client_id_set_;
	};

}