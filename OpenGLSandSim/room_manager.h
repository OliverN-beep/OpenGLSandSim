#pragma once

#include "room.h"
#include <vector>

class RoomManager
{
public:
	RoomManager() = default;

	void addRoom(const Room& room);

	Room& getCurrentRoom();
	void setCurrentRoom(int index);

	int getCurrentRoomIndex() const;
	int findRoomAtGridPosition(sf::Vector2i pos) const;

private:
	std::vector<Room> m_rooms;

	int m_currentRoomIndex = 0;
};