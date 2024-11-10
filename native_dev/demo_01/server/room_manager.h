#pragma once

#include <string>
#include <set>
#include <map>
#include <memory>

namespace yk {

class Room {
public:
	Room(const std::string& id);
	~Room();
private:
	std::string id_;
	std::set<std::string> members_;
};


class RoomManager {
public:
	static RoomManager* Instance() {
		static RoomManager self;
		return &self;
	}
	std::string CreateRoom(const std::string& caller_id, const std::string& callee_id);
private:
	RoomManager();
	~RoomManager();
	std::string GenerateRoomId(const std::string& caller_id, const std::string& callee_id);


	std::map<std::string, std::shared_ptr<Room>> rooms_;

};

}