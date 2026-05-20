#pragma once

#include <vector>
#include <SFML/Graphics.hpp>

enum class TileType
{
	Solid,
	Spike,
	Empty	// Keep "Empty" as last entry to avoid issues with indexing in drawTileHotbar() in game.cpp
};

class TileMap
{
public:
	// Constructor to initialize the tile map with given dimensions and tile size
	TileMap(int width, int height, int tileSize);

	void setTile(int x, int y, TileType type);		// Set the tile type at the specified (x, y) position
	TileType getTile(int x, int y) const;			// Get the tile type at the specified (x, y) position

	bool isSolid(int x, int y) const;				// Check if the tile at the specified (x, y) position is solid
	bool isSpike(int x, int y) const;				// Check if the tile at the specified (x, y) position is a spike

	void draw(sf::RenderWindow& window, sf::Vector2f offset = { 0.f, 0.f }) const;		// Draw the tile map onto the provided SFML render window

	int getWidth() const { return m_width; }		// Get the width of the tile map in tiles
	int getHeight() const { return m_height; }		// Get the height of the tile map in tiles
	int getTileSize() const { return m_tileSize; }	// Get the size of each tile in pixels

	TileType m_selectedTileType = TileType::Solid;	// Currently selected tile type for editing the tile map

	int getTileAtlasTexture(TileType type);			// Gets tilemap's atlas texture
	void updateTileMesh();							// Updates the tileset atlas mesh (simplifies draw call)

private:
	bool inBounds(int x, int y) const;				// Check if the (x, y) tile coordinates are within the bounds of the tile map

	int m_width;									// Width of the tile map in tiles
	int m_height;									// Height of the tile map in tiles
	int m_tileSize;									// Size of each tile in pixels
	sf::Texture m_tileset;							// Stores tileset atlas texture
	sf::VertexArray m_verticies;					// Vertex array for tileset

	bool m_tileMeshDirty = true;

	std::vector<std::vector<TileType>> m_tiles;		// 2D vector to store the tile types for the tile map
};