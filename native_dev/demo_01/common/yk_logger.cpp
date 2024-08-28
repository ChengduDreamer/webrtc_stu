#include "yk_logger.h"
#include "common.h"

YKLogger::EOutPutPosition YKLogger::out_position_ = YKLogger::EOutPutPosition::kFile;

YKLogger::YKLogger() {
	std::string log_flag = "yk_webrtc_client_01";
	std::wstring pathw = GetModulePathW(NULL);
	std::string path = ToUTF8(pathw);
	console_log_ptr_ = spdlog::stdout_color_mt(log_flag);
	console_log_ptr_->set_level(spdlog::level::info);
	if (EOutPutPosition::kFile == out_position_) {
		auto max_size = 1048576 * 5 * 10;
		auto max_files = 20;
		file_log_ptr_ = spdlog::rotating_logger_mt(log_flag + "_log", path + "\\yk_webrtc_client_01_log.txt", max_size, max_files);
		file_log_ptr_->set_level(spdlog::level::info);
		spdlog::flush_every(std::chrono::seconds(5));
	}
}

YKLogger::~YKLogger() {
	
}
