#pragma once
#include <string>

namespace yk {
	const std::string kSignalsMsgType_Hello = "hello";
	const std::string kSignalsMsgType_Call = "call";
	const std::string kSignalsMsgType_OtherNotOnline = "other_not_online";
	const std::string kSignalsMsgType_CreatedRoom = "created_room";
	const std::string kSignalsMsgType_Message = "message";   // 消息转发
	
	
	const std::string kCandidateSdpMidName = "sdpMid";
	const std::string kCandidateSdpMlineIndexName = "sdpMLineIndex";
	const std::string kCandidateSdpName = "candidate";
}