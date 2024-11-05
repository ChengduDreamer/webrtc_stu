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
		if (!ws_ptr_->Connect(*ctx_ptr_->GetIoPool().get(), "127.0.0.1", "21365", "/")) {
			YK_LOGE("SignalsClient ws connect error.");
		}
		// to do ÐÄÌø
	}
	
	void SignalsClient::SayHello() {
		if (!ws_ptr_) {
			return;
		}
		auto settings = Settings::GetInstance();
		nlohmann::json msg_json;
		msg_json["client_id"] = settings->client_id;
		msg_json["operation_type"] = kSignalsMsgType_Hello;
		std::string msg_str = msg_json.dump();
		ws_ptr_->AsyncSendBin(msg_str);
	}

	void SignalsClient::Call(const std::string& remote_id) {
		auto settings = Settings::GetInstance();
		nlohmann::json msg_json;
		msg_json["room_id"] = "1";
		msg_json["caller_client_id"] = settings->client_id;
		msg_json["callee_client_id"] = remote_id;
		msg_json["operation_type"] = kSignalsMsgType_Call;
		std::string msg_str = msg_json.dump();
		ws_ptr_->AsyncSendBin(msg_str);
	}
}