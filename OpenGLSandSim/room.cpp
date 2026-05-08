#include "room.h"

Room::Room(int width, int height, int cellSize, int tileSize, sf::Vector2i roomGridPosition, const std::string& roomName):
	m_world(width, height, cellSize),
	m_tileMap(width / (tileSize / cellSize), height / (tileSize / cellSize), tileSize),
	m_gridPosition(roomGridPosition)
{
	// Set the tile map for the world to enable particle collision detection
	m_world.setTileMap(&m_tileMap);
}

void Room::update()
{
	m_world.update();
}

void Room::draw(sf::RenderWindow& window) const
{
	m_world.draw(window);
	m_tileMap.draw(window);
}

World& Room::getWorld()
{
	return m_world;
}

TileMap& Room::getTileMap()
{
	return m_tileMap;
}

sf::Vector2i Room::getGridPosition() const
{
	return m_gridPosition;
}