#pragma once

#include <asio2/asio2.hpp>

namespace yk {
	class Context {
	public:
		static std::shared_ptr<Context> Make();
		Context();
		~Context();
		void Init();
		std::shared_ptr<asio2::iopool> GetIoPool();
	private:
		std::shared_ptr<asio2::iopool> iopool_ = nullptr;
	};

}