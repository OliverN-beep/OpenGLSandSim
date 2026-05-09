#include "room_manager.h"

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

int RoomManager::getRoomCount() const
{
	return static_cast<int>(m_rooms.size());
}

Room& RoomManager::getRoom(int index)
{
	return m_rooms[index];
}

const Room& RoomManager::getRoom(int index) const
{
	return m_rooms[index];
}