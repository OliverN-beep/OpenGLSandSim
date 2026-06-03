#pragma once

#include "world.h"
#include "tilemap.h"
#include "collectable.h"

#include <memory>

class Room
{
public:
	Room(
		int width,
		int height,
		int cellSize,
		int tileSize,
		sf::Vector2i roomGridPosition);

	void update();
	void draw(sf::RenderTarget& target, sf::Vector2f offset = { 0.f, 0.f });

	World& getWorld();
	TileMap& getTileMap();

	sf::Vector2i getGridPosition() const;

	const std::string& getName() const;

	Room(const Room&) = delete;
	Room& operator=(const Room&) = delete;

	Room(Room&&) noexcept = default;
	Room& operator=(Room&&) noexcept = default;

	json serialise() const;
	void deserialise(const json& data);

	std::vector<Collectable>& getCollectables();

private:
	World m_world;
	std::unique_ptr<TileMap> m_tileMap;

	sf::Vector2i m_gridPosition;

	std::string m_roomName;

	std::vector<Collectable> m_collectables;
};