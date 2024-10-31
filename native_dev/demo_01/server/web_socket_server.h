#pragma once
#include <string>
#include <memory>
#include <map>
#include <functional>
#include <asio2/websocket/ws_server.hpp>

namespace yk {
	using WSOnRecvMsgCallbackFunc = std::function<void(std::string)>;

	class WebSocketServer {
	public:
		static std::shared_ptr<WebSocketServer> Make();
		WebSocketServer();
		~WebSocketServer();
		void Init();
		void SetOnRecvMsgCallback(WSOnRecvMsgCallbackFunc&& cbk);
	private:
		void OnRecvMsg(const std::shared_ptr<asio2::ws_session>& session_ptr, std::string msg);
	private:
		std::shared_ptr<asio2::ws_server> ws_server_ = nullptr;
		std::string host_ = "0.0.0.0";
		std::string port_ = "21365";

		WSOnRecvMsgCallbackFunc on_recv_msg_callback_ = nullptr;
		std::map<std::string, std::shared_ptr<asio2::ws_session>> client_sessions_;
	};
}