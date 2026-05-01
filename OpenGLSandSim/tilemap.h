#pragma once

#include <vector>
#include <SFML/Graphics.hpp>

enum class TileType {
	Empty,
	Solid,
	Platform,
	Spike,
	// Add more tile types as needed
};

class TileMap
{
public:
	TileMap(int width, int height, int tileSize);	// Constructor to initialize the tile map with given dimensions and tile size

	void setTile(int x, int y, TileType type);		// Set the tile type at the specified (x, y) position
	TileType getTile(int x, int y) const;			// Get the tile type at the specified (x, y) position

	bool isSolid(int x, int y) const;				// Check if the tile at the specified (x, y) position is solid

	void draw(sf::RenderWindow& window) const;		// Draw the tile map onto the provided SFML render window

	int getWidth() const { return m_width; }		// Get the width of the tile map in tiles
	int getHeight() const { return m_height; }		// Get the height of the tile map in tiles
	int getTileSize() const { return m_tileSize; }	// Get the size of each tile in pixels

	bool isSolidAtParticle(int x, int y) const;		// Check if the tile at the specified (x, y) position is solid for particle collision detection

private:
	int index(int x, int y) const;					// Convert (x, y) tile coordinates to a linear index in the tile vector
	bool inBounds(int x, int y) const;				// Check if the (x, y) tile coordinates are within the bounds of the tile map

	int m_width;									// Width of the tile map in tiles
	int m_height;									// Height of the tile map in tiles
	int m_tileSize;									// Size of each tile in pixels

	std::vector<std::vector<TileType>> m_tiles;		// 2D vector to store the tile types for the tile map
};