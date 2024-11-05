#include "web_socket_client.h"
#include <NlohmannJson/json.hpp>
#include "yk_logger.h"

namespace yk {

	std::shared_ptr<WebSocketClient> WebSocketClient::Make(const std::string& name) {
		return std::make_shared<WebSocketClient>(name);
	}

	WebSocketClient::~WebSocketClient() {
		ws_client_ = nullptr;
	}

	bool WebSocketClient::Connect(asio2::iopool& io_ctx,const std::string& host,const std::string& port,const std::string& path) {
		ws_client_ = std::make_shared<asio2::ws_client>(io_ctx);
		ws_client_->bind_init([weak_thiz = weak_from_this()]() {
			if (auto thiz = weak_thiz.lock()) {
				// Set the binary message write option.
				thiz->ws_client_->ws_stream().binary(true);

				// how to set custom websocket request data : 
				thiz->ws_client_->ws_stream().set_option(
					websocket::stream_base::decorator([](websocket::request_type& req)
						{
							req.set(http::field::authorization, "websocket-client-authorization");
						}));
				YK_LOGI("{} ws init.", thiz->name_);
			}

		}).bind_connect([weak_thiz = weak_from_this()]() {
			if (auto thiz = weak_thiz.lock()) {
				auto ec = asio2::get_last_error();
				if (ec) {
					YK_LOGE("{} ws connect failure : {} {}", thiz->name_, asio2::last_error_val(), asio2::last_error_msg().c_str());
				}
				else {
					YK_LOGI("{} ws connect success : {} {}", thiz->name_, thiz->ws_client_->remote_address(), thiz->ws_client_->remote_port());
				}
				thiz->on_connected(ec);
			}

		}).bind_disconnect([weak_thiz = weak_from_this()]() {
			if (auto thiz = weak_thiz.lock()) {
				if (asio2::get_last_error()) {
					YK_LOGE("{} ws disconnect failure : {} {}", thiz->name_, asio2::last_error_val(), asio2::last_error_msg().c_str());
				}
				else {
					YK_LOGI("{} ws disconnect success : {} {}", thiz->name_, thiz->ws_client_->remote_address(), thiz->ws_client_->remote_port());
					thiz->on_closed();
				}
			}
		}).bind_upgrade([weak_thiz = weak_from_this()]() {
			if (auto thiz = weak_thiz.lock()) {
				if (asio2::get_last_error()) {
					YK_LOGE("{} ws upgrade failure : {} {}", thiz->name_, asio2::last_error_val(), asio2::last_error_msg());
				}
				else {
					YK_LOGI("{} ws upgrade success", thiz->name_);
				}
			}

		}).bind_recv([weak_thiz = weak_from_this()](std::string_view data) {
			if (auto thiz = weak_thiz.lock()) {
				thiz->on_message(data);
			}
		});

		if (!ws_client_->async_start(host, port, path)) {
			YK_LOGE("{} ws connect websocket server failure : {} {}", name_, asio2::last_error_val(), asio2::last_error_msg());
			return false;
		}
		return true;
	}

	void WebSocketClient::Close() {

		if(ws_client_) {
			ws_client_->set_auto_reconnect(false);
			//ws_client_->ws_stream().async_close(boost::beast::websocket::close_code::normal, [thiz_weak = weak_from_this()](::bho::beast::error_code ec) {
			//	if (auto thiz = thiz_weak.lock()) {
			//		YK_LOGI("async close ws client {} session finish. ec msg {}", thiz->name_, ec.message());
			//	}
			//});
			//ws_client_->ws_stream().close(boost::beast::websocket::close_code::normal);
			ws_client_->socket().close();
		}
	}

	void WebSocketClient::AsyncSendBin(const std::string& bin) {
		if (ws_client_) {
			++pending_messgae_current_count_;
			ws_client_->async_send(bin, [weak_thiz = weak_from_this()]() {
				if (auto thiz = weak_thiz.lock()) {
					--thiz->pending_messgae_current_count_;
				}
			});
		}
	}

} // namespace dl