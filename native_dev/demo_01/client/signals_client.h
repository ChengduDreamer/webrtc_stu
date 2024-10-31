#pragma once
#include <memory>
#include <string>
namespace yk {

	class WebSocketClient;
	class Context;

	class SignalsClient {
	public:
		SignalsClient(const std::shared_ptr<Context>& ctx_ptr);
		~SignalsClient();
		void SayHello();
		void Call(const std::string& remote_id);
	private:
		void Init();
		std::shared_ptr<WebSocketClient> ws_ptr_ = nullptr;
		std::shared_ptr<Context> ctx_ptr_ = nullptr;
	};

}
