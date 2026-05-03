#include "world.h"
#include "tilemap.h"

void World::setTileMap(TileMap* tileMap)
{
	m_tileMap = tileMap;
}

// Constructor to initialise the world with given dimensions and cell size
World::World(int width, int height, int cellSize)
	: m_width(width),
	m_height(height),
	m_cellSize(cellSize),
	cells(width * height)
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

bool World::canMoveInto(int x1, int y1, int x2, int y2)
{
	if (!inBounds(x2, y2))
		return false;

	Cell& cellA = getCellRef(x1, y1);
	Cell& cellB = getCellRef(x2, y2);

	auto& matA = g_materials[(int)cellA.material];
	auto& matB = g_materials[(int)cellB.material];

	if (cellB.material == MaterialType::Empty)
		return true;

	// A material can move into another material if it is denser than the other material
	return matA.density > matB.density || (matA.density == matB.density && rand() % 2 == 0);
}	

bool World::tryMove(int x1, int y1, int x2, int y2)
{
	if (!canMoveInto(x1, y1, x2, y2))
	{
		return false;
	}

	if (isTileBlocked(x2, y2))
	{
		return false;
	}

	Cell& cellA = getCellRef(x1, y1);
	Cell& cellB = getCellRef(x2, y2);

	std::swap(cellA, cellB);

	cellA.updateFrame = m_currentFrame;
	cellB.updateFrame = m_currentFrame;

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
			updateCellBehaviour(x, y);
		}
	}
}

void World::updateCellBehaviour(int x, int y)
{
	if (!inBounds(x, y))
		return;

	Cell& cell = getCellRef(x, y);

	if (cell.material == MaterialType::Empty)
		return;

	if (cell.updateFrame == m_currentFrame)
		return;

	MaterialProperties properties = g_materials[(int)cell.material];

	switch (properties.behaviour)
	{
		case BehaviourType::Powder:
			updatePowder(x, y);
			break;

		case BehaviourType::Liquid:
			updateLiquid(x, y);
			break;

		case BehaviourType::Gas:
			updateGas(x, y);
			break;

		case BehaviourType::None:
		default:
			cell.updateFrame = m_currentFrame;
			break;
	}
}

void World::updatePowder(int x, int y)
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

	// Draw each cell based on its material type
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			MaterialType matType = getCell(x, y);

			if (matType == MaterialType::Empty)
				continue;

			auto& mat = g_materials[(int)matType];
			rect.setFillColor(mat.colour);

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

			if (rand() % 100 < 99) // Randomly skip some cells for a more natural look
			{
				setCell(cx + dx, cy + dy, type);
			}
		}
	}
}