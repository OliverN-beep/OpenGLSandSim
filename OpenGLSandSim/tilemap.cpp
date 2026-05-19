#include "tilemap.h"
#include <iostream>

TileMap::TileMap(int width, int height, int tileSize) :
	m_width(width),
	m_height(height),
	m_tileSize(tileSize),
	m_tiles(width, std::vector<TileType>(height, TileType::Empty))
{
	if (!m_tileset.loadFromFile("tilesets/mono_tileset_d.png"))
		printf("error loading tileset atlas\n");

	m_tileset.setSmooth(false);

	m_verticies.setPrimitiveType(sf::PrimitiveType::Triangles);
	m_verticies.resize(m_width * m_height * 6);
}

bool TileMap::inBounds(int x, int y) const
{
	// Check if the (x, y) tile coordinates are within the bounds of the tile map
	return x >= 0 && x < m_width && y >= 0 && y < m_height;
}

void TileMap::setTile(int x, int y, TileType type)
{
	// Set the tile type at the specified (x, y) position
	if (inBounds(x, y))
	{
		// Set the tile type in the 2D vector
		m_tiles[x][y] = type;
	}

	m_tileMeshDirty = true;
}

TileType TileMap::getTile(int x, int y) const
{
	// Get the tile type at the specified (x, y) position
	if (inBounds(x, y))
	{
		// Return the tile type from the 2D vector
		return m_tiles[x][y];
	}

	return TileType::Empty; // Return Empty if out of bounds
}

bool TileMap::isSolid(int x, int y) const
{
	TileType type = getTile(x, y);
	return type == TileType::Solid;
}

bool TileMap::isSpike(int x, int y) const
{
	TileType type = getTile(x, y);
	return type == TileType::Spike;
}

int TileMap::getTileAtlasTexture(TileType type)
{
	switch (type)
	{
		case TileType::Solid: return 0;
		case TileType::Spike: return 1;

		default: return -1;
	}
}

void TileMap::updateTileMesh()
{
	int atlasColumns = m_tileset.getSize().x / m_tileSize;

	// Draw the tile map onto the provided SFML render window
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			TileType type = getTile(x, y);

			int tileIndex = getTileAtlasTexture(type);
			int vertexIndex = (x + y * m_width) * 6;

			sf::Vertex* tri = &m_verticies[vertexIndex];

			if (tileIndex == -1)
			{
				for (int i = 0; i < 6; ++i)
				{
					tri[i].color = sf::Color::Transparent;
				}

				continue;
			}

			int tu = tileIndex % atlasColumns;
			int tv = tileIndex / atlasColumns;

			float tx = static_cast<float>(tu * m_tileSize);
			float ty = static_cast<float>(tv * m_tileSize);

			float px = static_cast<float>(x * m_tileSize);
			float py = static_cast<float>(y * m_tileSize);

			float s = static_cast<float>(m_tileSize);

			// Triangle 1
			tri[0].position = { px, py };
			tri[1].position = { px + s, py };
			tri[2].position = { px + s, py + s };

			// Triangle 2
			tri[3].position = { px, py };
			tri[4].position = { px + s, py + s };
			tri[5].position = { px, py + s };

			// Texture coordinates
			tri[0].texCoords = { tx, ty };
			tri[1].texCoords = { tx + s, ty };
			tri[2].texCoords = { tx + s, ty + s };
			tri[3].texCoords = { tx, ty };
			tri[4].texCoords = { tx + s, ty + s };
			tri[5].texCoords = { tx, ty + s };
		}
	}

	m_tileMeshDirty = false;
}

void TileMap::draw(sf::RenderWindow& window, sf::Vector2f offset) const
{
	if (m_tileMeshDirty)
		const_cast<TileMap*>(this)->updateTileMesh();

	sf::RenderStates states;
	states.texture = &m_tileset;

	states.transform.translate(offset);

	window.draw(m_verticies, states);
}