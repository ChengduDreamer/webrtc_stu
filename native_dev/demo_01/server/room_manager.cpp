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

	room_ptr->caller_id_ = caller_id;

	room_ptr->callee_id_ = callee_id;

	rooms_[room_id] = room_ptr;

	return room_id;
}

std::shared_ptr<Room> RoomManager::GetRoom(const std::string& room_id) {

	if (rooms_.count(room_id) <= 0) {
		return nullptr;
	}
	return rooms_[room_id];
}


}