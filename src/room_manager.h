#pragma once

#include <vector>
#include <utility>
#include <fstream>
#include <nlohmann/json.hpp>
#include "room.h"

class RoomManager
{
public:
	int m_roomWidth;
	int m_roomHeight;
	int m_cellSize;
	int m_tileSize;

public:
	// Constructor
	RoomManager(
		int roomWidth,
		int roomHeight,
		int cellSize,
		int tileSize
	);

	void createRoom(sf::Vector2i gridPos);
	void deleteCurrentRoom();

	Room& getCurrentRoom();
	void setCurrentRoom(int index);

	int getCurrentRoomIndex() const;
	int findRoomAtGridPosition(sf::Vector2i pos) const;

	int getRoomCount() const;

	Room& getRoom(int index);
	const Room& getRoom(int index) const;

	bool hasRoomAt(sf::Vector2i gridPos) const;

	void saveWorld(const std::string& filename);
	void loadWorld(const std::string& filename);

private:
	std::vector<Room> m_rooms;

	int m_currentRoomIndex = 0;
};