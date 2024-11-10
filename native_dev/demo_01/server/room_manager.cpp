#include "room_manager.h"



namespace yk {

Room::Room(const std::string& id) : id_(id) {
	
}

Room::~Room() {
	
}

RoomManager::RoomManager() {
	
}

RoomManager::~RoomManager() {
	
}

std::string RoomManager::GenerateRoomId(const std::string& caller_id, const std::string& callee_id) {
	return caller_id + "_" + callee_id;
}

std::string RoomManager::CreateRoom(const std::string& caller_id, const std::string& callee_id) {
	std::string room_id = GenerateRoomId(caller_id, callee_id);
	
	std::shared_ptr<Room> room_ptr = std::make_shared<Room>(room_id);

	rooms_[room_id] = room_ptr;

	return room_id;
}

}