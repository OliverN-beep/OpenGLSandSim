#include "room_manager.h"

RoomManager::RoomManager(
	int roomWidth,
	int roomHeight,
	int cellSize,
	int tileSize)
	: m_roomWidth(roomWidth),
	m_roomHeight(roomHeight),
	m_cellSize(cellSize),
	m_tileSize(tileSize)
{
	// Create initial room
	createRoom({ 0, 0 });
}

void RoomManager::createRoom(sf::Vector2i gridPos)
{
	if (hasRoomAt(gridPos))
	{
		return;
	}

	m_rooms.emplace_back(
		m_roomWidth,
		m_roomHeight,
		m_cellSize,
		m_tileSize,
		gridPos
	);
}

void RoomManager::deleteCurrentRoom()
{
	if (m_rooms.size() <= 1)
	{
		return;
	}

	m_rooms.erase(m_rooms.begin() + m_currentRoomIndex);

	if (m_currentRoomIndex >= m_rooms.size())
	{
		m_currentRoomIndex = static_cast<int>(m_rooms.size()) - 1;
	}

	printf("ROOM DELETED\n");
}

Room& RoomManager::getCurrentRoom()
{
	assert(!m_rooms.empty());

	return m_rooms[m_currentRoomIndex];
}

void RoomManager::setCurrentRoom(int index)
{
	if (index >= 0 && index < static_cast<int>(m_rooms.size()))
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

bool RoomManager::hasRoomAt(sf::Vector2i gridPos) const
{
	return findRoomAtGridPosition(gridPos) != -1;
}

void RoomManager::saveWorld(const std::string& filename)
{
	json world;

	world["rooms"] = json::array();

	for (const auto& room : m_rooms)
	{
		world["rooms"].push_back(room.serialise());
	}

	std::ofstream file(filename);

	file << world.dump(4);
}

void RoomManager::loadWorld(const std::string& filename)
{
	std::ifstream file(filename);

	if (!file.is_open())
		return;

	json world;

	file >> world;

	m_rooms.clear();

	for (const auto& roomData : world["rooms"])
	{
		Room room(m_roomWidth, m_roomHeight, m_cellSize, m_tileSize, { 0, 0 });

		room.deserialise(roomData);

		m_rooms.push_back(std::move(room));
	}
}