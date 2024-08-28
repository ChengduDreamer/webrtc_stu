#pragma once
#include <string>
#include <memory>
#include <asio2/websocket/ws_server.hpp>

namespace yk {
	class WebSocketServer {
	public:
		static std::shared_ptr<WebSocketServer> Make();
		WebSocketServer();
		~WebSocketServer();
		void Init();
	private:
		std::shared_ptr<asio2::ws_server> ws_server_ = nullptr;
		std::string host_ = "0.0.0.0";
		std::string port_ = "21365";

	};
}