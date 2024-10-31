#pragma once
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace spdlog {
	class logger;
}

class YKLogger {
public:
	enum class EOutPutPosition {
		kStdOut,
		kFile
	};
	static EOutPutPosition out_position_;
	static YKLogger* Instance() {
		static YKLogger self;
		return &self;
	}
	std::shared_ptr<spdlog::logger> console_log_ptr_ = nullptr;
	std::shared_ptr<spdlog::logger> file_log_ptr_ = nullptr;
	~YKLogger();
private:
	YKLogger();
};



namespace yk {
	template<typename... Args>
	void LogE(Args&& ...args) {
		//YKLogger::Instance()->console_log_ptr_->error(std::forward<Args>(args)...);
		//if (YKLogger::Instance()->file_log_ptr_) {
		//	YKLogger::Instance()->file_log_ptr_->error(std::forward<Args>(args)...);
		//}
	}

	template<typename... Args>
	void LogW(Args&& ...args) {
		//YKLogger::Instance()->console_log_ptr_->warn(std::forward<Args>(args)...);
		//if (YKLogger::Instance()->file_log_ptr_) {
		//	YKLogger::Instance()->file_log_ptr_->warn(std::forward<Args>(args)...);
		//}
	}

	template<typename... Args>
	void LogI(Args&& ...args) {
		//YKLogger::Instance()->console_log_ptr_->info(std::forward<Args>(args)...);
		//if (YKLogger::Instance()->file_log_ptr_) {
		//	YKLogger::Instance()->file_log_ptr_->info(std::forward<Args>(args)...);
		//}
	}

	template<typename... Args>
	void LogD(Args&& ...args) {
		//YKLogger::Instance()->console_log_ptr_->debug(std::forward<Args>(args)...);
		//if (YKLogger::Instance()->file_log_ptr_) {
		//	YKLogger::Instance()->file_log_ptr_->debug(std::forward<Args>(args)...);
		//}
	}
}
//std::forward<Args>(args)... 是一个参数包展开的表达式。它将参数包 args 中的每个参数进行展开，并将展开后的参数传递给 目标函数。
