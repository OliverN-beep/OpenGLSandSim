#pragma once

#include "world.h"
#include "tilemap.h"

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

private:
	World m_world;
	TileMap m_tileMap;

	sf::Vector2i m_gridPosition;

	std::string m_roomName;
};