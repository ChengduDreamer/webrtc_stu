#include "context.h"

namespace yk {

	std::shared_ptr<Context> Context::Make() {
		return std::make_shared<Context>();
	}

	Context::Context() {
		Init();
	}

	Context::~Context() {
	
	}

	void Context::Init() {
		iopool_ = std::make_shared<asio2::iopool>();
		iopool_->start();
	}

	std::shared_ptr<asio2::iopool> Context::GetIoPool() {
		return iopool_;
	}
}