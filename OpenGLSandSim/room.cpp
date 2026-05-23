#include "room.h"

Room::Room(
	int width,
	int height,
	int cellSize,
	int tileSize,
	sf::Vector2i roomGridPosition,
	const std::string& roomName):
	
	m_world(width, height, cellSize),
	m_tileMap((width * cellSize) / tileSize, (height * cellSize) / tileSize, tileSize),
	m_gridPosition(roomGridPosition)
{
	// Set the tile map for the world to enable particle collision detection
	m_world.setTileMap(&m_tileMap);

	printf("Tile size: %d\n", m_tileMap.getTileSize());
}

void Room::update()
{
	m_world.update();
}

void Room::draw(sf::RenderTarget& target, sf::Vector2f offset)
{
	m_tileMap.draw(target, offset);
	m_world.draw(target, offset);
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

const std::string& Room::getName() const
{
	return m_roomName;
}