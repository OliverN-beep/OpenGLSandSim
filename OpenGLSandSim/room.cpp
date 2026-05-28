#include "room.h"

Room::Room(
	int width,
	int height,
	int cellSize,
	int tileSize,
	sf::Vector2i roomGridPosition)
	: m_world(width, height, cellSize),
	m_tileMap(std::make_unique<TileMap>((width * cellSize) / tileSize, (height * cellSize) / tileSize, tileSize)),
	m_gridPosition(roomGridPosition)
{
	// Set the tile map for the world to enable particle collision detection
	m_world.setTileMap(m_tileMap.get());

	// Initialise collectables
	m_collectables.emplace_back(sf::Vector2f(200.f, 100.f));

	printf("Tile size: %d\n", m_tileMap->getTileSize());
}

void Room::update()
{
	m_world.update();
}

void Room::draw(sf::RenderTarget& target, sf::Vector2f offset)
{
	m_tileMap->draw(target, offset);
	m_world.draw(target, offset);

	// Draw collectables
	for (const auto& collectable : m_collectables)
	{
		collectable.draw(target);
	}
}

World& Room::getWorld()
{
	return m_world;
}

TileMap& Room::getTileMap()
{
	return *m_tileMap;
}

sf::Vector2i Room::getGridPosition() const
{
	return m_gridPosition;
}

const std::string& Room::getName() const
{
	return m_roomName;
}

json Room::serialise() const
{
	json data;

	data["gridX"] = m_gridPosition.x;
	data["gridY"] = m_gridPosition.y;

	data["tilemap"] = m_tileMap->serialise();

	return data;
}

void Room::deserialise(const json& data)
{
	m_gridPosition.x = data["gridX"];
	m_gridPosition.y = data["gridY"];

	m_tileMap->deserialise(data["tilemap"]);
}

std::vector<Collectable>& Room::getCollectables()
{
	return m_collectables;
}