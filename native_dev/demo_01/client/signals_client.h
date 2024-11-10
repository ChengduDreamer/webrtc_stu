#pragma once
#include <memory>
#include <string>
#include <functional>
#include <NlohmannJson/json.hpp>
#include "common/task_worker_callback.h"

namespace yk {

	class WebSocketClient;
	class Context;

	class SignalsClient {
	public:

		using OnCreatedRoomMsgCallbackFunc = std::function<void(void)>;

		SignalsClient(const std::shared_ptr<Context>& ctx_ptr);
		~SignalsClient();
		void SayHello();
		void Call(const std::string& remote_id);

		void RecvMsg(const std::string msg);
		void HandleMsg(const std::string msg);
		void HandleCreatedRoomMsg(const nlohmann::json& jsobj);

		void SetOnCreatedRoomMsgCallback(OnCreatedRoomMsgCallbackFunc&& cbk) {
			on_created_room_msg_callback_ = std::move(cbk);
		}

	private:
		void Init();
		std::shared_ptr<WebSocketClient> ws_ptr_ = nullptr;
		std::shared_ptr<Context> ctx_ptr_ = nullptr;

		YKTaskWorker task_worker_;

		OnCreatedRoomMsgCallbackFunc on_created_room_msg_callback_;
	};

}
