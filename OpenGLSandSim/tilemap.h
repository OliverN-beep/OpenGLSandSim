#pragma once

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <json/json.hpp>

using json = nlohmann::json;

enum class TileType
{
	Solid,
	Spike,
	Bounce,
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
	bool isBounce(int x, int y) const;				// Check if the tile at the specified (x, y) position is a bounce pad

	void draw(sf::RenderTarget& target, sf::Vector2f offset = { 0.f, 0.f }) const;		// Draw the tile map onto the provided SFML render window

	int getWidth() const { return m_width; }		// Get the width of the tile map in tiles
	int getHeight() const { return m_height; }		// Get the height of the tile map in tiles
	int getTileSize() const { return m_tileSize; }	// Get the size of each tile in pixels (CRASHES: READ ACCESS VIOLATION maybe to do with 0 bug from world.cpp?)

	TileType m_selectedTileType = TileType::Solid;	// Currently selected tile type for editing the tile map

	int getTileAtlasIndex(TileType type);			// Gets tilemap's atlas texture
	void updateTileMesh();							// Updates the tileset atlas mesh (simplifies draw call)

	const sf::Texture& getTileset() const { return m_tileset; }	// Gets our tileset

	// JSON stuff
	json serialise() const;
	void deserialise(const json& data);

private:
	bool inBounds(int x, int y) const;				// Check if the (x, y) tile coordinates are within the bounds of the tile map

	int m_width;									// Width of the tile map in tiles
	int m_height;									// Height of the tile map in tiles
	int m_tileSize;									// Size of each tile in pixels
	sf::Texture m_tileset;							// Stores tileset atlas texture
	sf::VertexArray m_vertices;						// Vertex array for tileset

	bool m_tileMeshDirty = true;

	std::vector<std::vector<TileType>> m_tiles;		// 2D vector to store the tile types for the tile map
};