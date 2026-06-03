#include "tilemap.h"

TileMap::TileMap(int width, int height, int tileSize) :
	m_width(width),
	m_height(height),
	m_tileSize(tileSize),
	m_tiles(width, std::vector<TileType>(height, TileType::Empty))
{
	if (!m_tileset.loadFromFile("assets/tilesets/mono_tileset_d.png"))
		printf("error loading tileset atlas\n");

	m_tileset.setSmooth(false);

	m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
	m_vertices.resize(static_cast<size_t>(m_width) * static_cast<size_t>(m_height * 6));
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

bool TileMap::isBounce(int x, int y) const
{
	TileType type = getTile(x, y);
	return type == TileType::Bounce;
}

int TileMap::getTileAtlasIndex(TileType type)
{
	switch (type)
	{
	case TileType::Solid: return 0;
	case TileType::Spike: return 1;
	case TileType::Bounce: return 2;
	
	// Fill with empty otherwise
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

			int tileIndex = getTileAtlasIndex(type);
			int vertexIndex = (x + y * m_width) * 6;

			sf::Vertex* tri = &m_vertices[vertexIndex];

			if (tileIndex == -1)
			{
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

				for (int i = 0; i < 6; ++i)
				{
					tri[i].color = sf::Color::Transparent;
					tri[i].texCoords = { 0.f, 0.f };
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

			// Initialise colour to avoid transparency bug
			for (int i = 0; i < 6; ++i)
			{
				tri[i].color = sf::Color::White;
			}
		}
	}

	m_tileMeshDirty = false;
}

void TileMap::draw(sf::RenderTarget& target, sf::Vector2f offset) const
{
	if (m_tileMeshDirty)
		const_cast<TileMap*>(this)->updateTileMesh();

	sf::RenderStates states;
	states.texture = &m_tileset;

	states.transform.translate({ std::floor(offset.x), std::floor(offset.y) });

	target.draw(m_vertices, states);
}

json TileMap::serialise() const
{
	json data;

	data["width"] = m_width;
	data["height"] = m_height;

	data["tiles"] = json::array();

	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			data["tiles"][y][x] = static_cast<int>(m_tiles[x][y]);
		}
	}

	return data;
}

void TileMap::deserialise(const json& data)
{
	if (!data.contains("tiles"))
	{
		printf("Missing tile data\n");
		return;
	}

	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			m_tiles[x][y] = static_cast<TileType>(data["tiles"][y][x].get<int>());
		}
	}

	m_tileMeshDirty = true;
}