#pragma once
#include <string>
#include <asio2/asio2.hpp>
#include <boost/signals2/signal.hpp>

namespace yk {

	namespace sig = boost::signals2;
	using sig::signal;
} //

namespace yk {

	class WebSocketClient : public std::enable_shared_from_this<WebSocketClient> {
	public:
		static std::shared_ptr<WebSocketClient> Make(const std::string& name);
		WebSocketClient(const std::string& name) : name_(name) {
		
		}
		virtual ~WebSocketClient();
		bool Connect(asio2::iopool& io_ctx,const std::string& host,const std::string& port,const std::string& path);
		void Close();
		void AsyncSendBin(const std::string& bin);
	
		void set_pending_message_max_count(int count) { pending_message_max_count_ = count; }

		signal<void(asio2::error_code)> on_connected;
		signal<void(std::string)> on_message;
		signal<void()> on_closed;
		int GetPendingMessgaeCurrentCount() {
			return pending_messgae_current_count_;
		}
	private:
		std::string name_;
		std::shared_ptr<asio2::ws_client> ws_client_;
		int pending_message_max_count_ = 4096;
		std::atomic_int pending_messgae_current_count_ = {0};
	};

} // namespace dl