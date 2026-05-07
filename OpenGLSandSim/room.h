#pragma once

#include "world.h"
#include "tilemap.h"

class Room
{
public:
	Room(int width,
		 int height,
		 int cellSize,
		 int tileSize,
		 sf::Vector2i roomGridPosition);

	void update();
	void draw(sf::RenderWindow& window) const;

	World& getWorld();
	TileMap& getTileMap();

	sf::Vector2i getGridPosition() const;

private:
	World m_world;
	TileMap m_tileMap;

	sf::Vector2i m_gridPosition;
};