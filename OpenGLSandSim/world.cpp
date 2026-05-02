#include "world.h"
#include "tilemap.h"

void World::setTileMap(TileMap* tileMap)
{
	m_tileMap = tileMap;
}

World::World(int width, int height, int cellSize)
	: m_width(width), m_height(height), m_cellSize(cellSize), cells(width * height)
{
}

int World::index(int x, int y) const
{
	return y * m_width + x;
}

bool World::inBounds(int x, int y) const
{
	return x >= 0 && x < m_width && y >= 0 && y < m_height;
}

Cell& World::getCellRef(int x, int y)
{
	return cells[index(x, y)];
}

const Cell& World::getCellRef(int x, int y) const
{
	return cells[index(x, y)];
}

MaterialType World::getCell(int x, int y) const
{
	if (!inBounds(x, y))
	{
		return MaterialType::Stone;
	}
	return getCellRef(x, y).material;
}

void World::setCell(int x, int y, MaterialType matType)
{
	if (inBounds(x, y))
	{
		getCellRef(x, y).material = matType;
	}
}

bool World::isEmpty(int x, int y) const
{
	return getCell(x, y) == MaterialType::Empty;
}

bool World::tryMove(int x, int y, int newX, int newY)
{
	if (!inBounds(newX, newY))
	{
		return false;
	}

	if (isTileBlocked(newX, newY))
	{
		return false;
	}

	if (!isEmpty(newX, newY))
	{
		return false;
	}

	getCellRef(newX, newY) = getCellRef(x, y);
	getCellRef(newX, newY).updateFrame = m_currentFrame;

	getCellRef(x, y).material = MaterialType::Empty;

	return true;
}

bool World::isTileBlocked (int x, int y) const
{
	if (!m_tileMap)
		return false;

	float worldX = static_cast<float>(x * m_cellSize);
	float worldY = static_cast<float>(y * m_cellSize);

	int tileX = static_cast<int>(worldX / m_tileMap->getTileSize());
	int tileY = static_cast<int>(worldY / m_tileMap->getTileSize());
	
	return m_tileMap->isSolid(tileX, tileY);
}

void World::update()
{
	++m_currentFrame;

	// Falling materials
	for (int y = m_height - 1; y >= 0; --y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			updateCell(x, y);
		}
	}
}

void::World::updateCell(int x, int y)
{
	if (!inBounds(x, y))
		return;

	Cell& cell = getCellRef(x, y);

	if (cell.material == MaterialType::Empty)
		return;

	if (cell.updateFrame == m_currentFrame)
		return;

	switch (cell.material)
	{
	case MaterialType::Sand:
	case MaterialType::Salt:
	case MaterialType::Fire:
	case MaterialType::Snow:
		updateSolid(x, y);
		break;

	case MaterialType::Water:
	case MaterialType::Oil:
		updateLiquid(x, y);
		break;

	case MaterialType::Smoke:
		updateGas(x, y);
		break;

	default:
		// For other materials, just mark them as updated for this frame
		cell.updateFrame = m_currentFrame;
		break;
	}
}

void World::updateSolid(int x, int y)
{
	// Try to move down
	if (tryMove(x, y, x, y + 1)) return;

	// Try to move down-left
	if (tryMove(x, y, x - 1, y + 1)) return;

	// Try to move down-right
	if (tryMove(x, y, x + 1, y + 1)) return;

	// If none of the moves were possible, mark the cell as updated for this frame
	getCellRef(x, y).updateFrame = m_currentFrame;
}

void World::updateLiquid(int x, int y)
{
	// Try to move down
	if (tryMove(x, y, x, y + 1)) return;

	// Try to move down-left
	if (tryMove(x, y, x - 1, y + 1)) return;

	// Try to move down-right
	if (tryMove(x, y, x + 1, y + 1)) return;

	// Try to move left
	if (tryMove(x, y, x - 1, y)) return;

	// Try to move right
	if (tryMove(x, y, x + 1, y)) return;

	// If none of the moves were possible, mark the cell as updated for this frame
	getCellRef(x, y).updateFrame = m_currentFrame;
}

void World::updateGas(int x, int y)
{
	// Try to move up
	if (tryMove(x, y, x, y - 1)) return;

	// Try to move up-left
	if (tryMove(x, y, x - 1, y - 1)) return;

	// Try to move up-right
	if (tryMove(x, y, x + 1, y - 1)) return;

	// If none of the moves were possible, mark the cell as updated for this frame
	getCellRef(x, y).updateFrame = m_currentFrame;
}

void World::draw(sf::RenderWindow& window) const
{
	// Create a rectangle shape to represent each cell
	sf::RectangleShape rect(sf::Vector2f((float)m_cellSize, (float)m_cellSize));

	// Define colors for different materials
	sf::Color SandYellow(194, 178, 128);
	sf::Color StoneGrey(128, 128, 128);
	sf::Color WaterBlue(0, 80, 255);
	sf::Color OilBlack(0, 0, 0);
	sf::Color FireRed(255, 0, 0);
	sf::Color SmokeGrey(105, 105, 105);
	sf::Color SnowWhite(255, 250, 250);
	sf::Color WoodBrown(139, 69, 19);
	sf::Color SaltWhite(255, 255, 255);

	// Draw each cell based on its material type
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			MaterialType matType = getCell(x, y);

			if (matType == MaterialType::Empty)
				continue;

			if (matType == MaterialType::Sand)
				rect.setFillColor(SandYellow);
			else if (matType == MaterialType::Stone)
				rect.setFillColor(StoneGrey);
			else if (matType == MaterialType::Water)
				rect.setFillColor(WaterBlue);
			else if (matType == MaterialType::Oil)
				rect.setFillColor(OilBlack);
			else if (matType == MaterialType::Fire)
				rect.setFillColor(FireRed);
			else if (matType == MaterialType::Smoke)
				rect.setFillColor(SmokeGrey);
			else if (matType == MaterialType::Snow)
				rect.setFillColor(SnowWhite);
			else if (matType == MaterialType::Wood)
				rect.setFillColor(WoodBrown);
			else if (matType == MaterialType::Salt)
				rect.setFillColor(SaltWhite);

			rect.setPosition(sf::Vector2f((float)(x * m_cellSize), (float)(y * m_cellSize)));
			window.draw(rect);
		}
	}
}

// Helper function for painting cells with a brush of a given size and material type
void World::paintCircle(int cx, int cy, int radius, MaterialType type)
{
	for (int dy = -radius; dy <= radius; ++dy)
	{
		for (int dx = -radius; dx <= radius; ++dx)
		{
			if (dx * dx + dy * dy > radius * radius)
				continue;

			if (rand() % 100 < 2) // Randomly skip some cells for a more natural look
			{
				setCell(cx + dx, cy + dy, type);
			}
		}
	}
}