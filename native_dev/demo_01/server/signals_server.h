#pragma once
#include <memory>
#include <string>
#include <map>
#include <set>

#include <NlohmannJson/json.hpp>

#include "common/task_worker_callback.h"

namespace yk {

	class WebSocketServer;

	class SignalsServer {
	public:
		SignalsServer();
		~SignalsServer();
	private:
		void Init();
		void OnRecvMsg(const std::string& msg);
		void ShowAllClientId();
		void HandleMsg(const std::string msg);
		void HandleCallMsg(const nlohmann::json& jsobj);
		void HandleForwardMsg(const nlohmann::json& jsobj);
		


		std::shared_ptr<WebSocketServer> websocket_server_ptr_ = nullptr;

		
		std::set<std::string> client_id_set_;


		YKTaskWorker task_worker_;
	};

}