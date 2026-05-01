#include "tilemap.h"

TileMap::TileMap(int width, int height, int tileSize) :
	m_width(width),
	m_height(height),
	m_tileSize(tileSize),
	m_tiles(width, std::vector<TileType>(height, TileType::Empty))
{
}

int TileMap::index(int x, int y) const
{
	// Convert (x, y) tile coordinates to a linear index in the tile vector
	return y * m_width + x;
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
	return type == TileType::Solid || type == TileType::Platform; // Consider Solid and Platform as solid tiles
}

void TileMap::draw(sf::RenderWindow& window) const
{
	sf::RectangleShape tileShape(sf::Vector2f(static_cast<float>(m_tileSize), static_cast<float>(m_tileSize)));

	// Draw the tile map onto the provided SFML render window
	for (int x = 0; x < m_width; ++x)
	{
		for (int y = 0; y < m_height; ++y)
		{
			TileType type = getTile(x, y);

			switch (type)
			{
			case TileType::Empty:
				tileShape.setFillColor(sf::Color::Transparent);
				break;
			case TileType::Solid:
				tileShape.setFillColor(sf::Color::Magenta);
				break;
			case TileType::Platform:
				tileShape.setFillColor(sf::Color(139, 69, 19)); // Brown color for platform
				break;
			case TileType::Spike:
				tileShape.setFillColor(sf::Color::Red);
				break;
			default:
				tileShape.setFillColor(sf::Color::Green); // Unknown tile type
				break;
			}

			tileShape.setPosition(sf::Vector2f((float)(x * m_tileSize), (float)(y * m_tileSize)));

			window.draw(tileShape);
		}
	}
}