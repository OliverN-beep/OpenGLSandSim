#pragma once

#include "room.h"
#include <vector>
#include <utility>

class RoomManager
{
public:
	RoomManager() = default;

	template<typename... Args>
	void addRoom(Args&&... args)
	{
		m_rooms.emplace_back(std::forward<Args>(args)...);
	}

	Room& getCurrentRoom();
	void setCurrentRoom(int index);

	int getCurrentRoomIndex() const;
	int findRoomAtGridPosition(sf::Vector2i pos) const;

	int getRoomCount() const;
	Room& getRoom(int index);
	const Room& getRoom(int index) const;

private:
	std::vector<Room> m_rooms;

	int m_currentRoomIndex = 0;
};