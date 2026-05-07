#include "room_manager.h"

void RoomManager::addRoom(const Room& room)
{
	m_rooms.push_back(room);
}

Room& RoomManager::getCurrentRoom()
{
	return m_rooms[m_currentRoomIndex];
}

void RoomManager::setCurrentRoom(int index)
{
	if (index >= 0 && index < m_rooms.size())
	{
		m_currentRoomIndex = index;
	}
}

int RoomManager::getCurrentRoomIndex() const
{
	return m_currentRoomIndex;
}

int RoomManager::findRoomAtGridPosition(sf::Vector2i pos) const
{
	for (int i = 0; i < m_rooms.size(); ++i)
	{
		if (m_rooms[i].getGridPosition() == pos)
		{
			return i;
		}
	}

	return -1;
}