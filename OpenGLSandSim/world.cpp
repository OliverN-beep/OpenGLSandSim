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
	// The number 6 represents 6 vertices (2 triangles)
	m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
	m_vertices.resize(static_cast<size_t>(m_width) * m_height * 6);

	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			int index = (x + y * m_width) * 6;

			sf::Vertex* tri = &m_vertices[index];

			float px = static_cast<float>(x * m_cellSize);
			float py = static_cast<float>(y * m_cellSize);
			float s = static_cast<float>(m_cellSize);

			tri[0].position = { px, py };
			tri[1].position = { px + s, py };
			tri[2].position = { px + s, py + s };

			tri[3].position = { px, py };
			tri[4].position = { px + s, py + s };
			tri[5].position = { px, py + s };
		}
	}
}

int World::index(int x, int y) const
{
	return y * m_width + x;
}

bool World::inBounds(int x, int y) const
{
	return x >= 0 && x < m_width && y >= 0 && y < m_height;
}

// Get a reference to the cell at the given coordinates (for read-write access)
Cell& World::getCellRef(int x, int y)
{
	return cells[index(x, y)];
}

// Get a const reference to the cell at the given coordinates (for read-only access)
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

// Set the material type of a specific cell at (x, y) to the given material type
void World::setCell(int x, int y, MaterialType matType)
{
    if (inBounds(x, y))
    {
        getCellRef(x, y).material = matType;
        m_isDirty = true;
    }
}

bool World::isEmpty(int x, int y) const
{
	return getCellRef(x, y).material == MaterialType::Empty;
}

bool World::canMoveInto(int x1, int y1, int x2, int y2)
{
	if (!inBounds(x2, y2))
		return false;

	Cell& cellA = getCellRef(x1, y1);
	Cell& cellB = getCellRef(x2, y2);

	auto& matA = g_materials[static_cast<int>(cellA.material)];
	auto& matB = g_materials[static_cast<int>(cellB.material)];

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

	// Update the updateFrame for both cells to indicate that they have been updated in the current frame
	cellA.updateFrame = m_currentFrame;
	cellB.updateFrame = m_currentFrame;

	// Mark world as dirty
	m_isDirty = true;

	return true;
}

bool World::isTileBlocked(int x, int y) const
{
	if (!m_tileMap)
		return false;

	// This occurs because m_tileSize is storing 0 initially for some reason?
	if (m_tileMap->getTileSize() == 0)
	{
		printf("Error: Int div by 0 (tileSize is initialised to 0)\n");
		return true;	// Return failure (makes sand stop falling)
	}

	int worldX = x * m_cellSize;
	int worldY = y * m_cellSize;

	int tileX = (x * m_cellSize) / m_tileMap->getTileSize();
	int tileY = (y * m_cellSize) / m_tileMap->getTileSize();
	
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

// NOTE FOR updateCellBehaviour(), updatePowder(), updateLiquid(), and updateGas()
// The order in which operations are performed DOES matter greatly

void World::updateCellBehaviour(int x, int y)
{
	if (!inBounds(x, y))
		return;

	Cell& cell = getCellRef(x, y);

	if (cell.material == MaterialType::Empty)
		return;

	if (cell.updateFrame == m_currentFrame)
		return;

	MaterialProperties properties = g_materials[static_cast<int>(cell.material)];

	// Apply gravity to the cell's velocity
	cell.velocity.y += properties.gravity;

	// Clamp the cell's velocity to the maximum allowed velocity for its material
	if (cell.velocity.y > properties.maxVelocity)
		cell.velocity.y = properties.maxVelocity;
	
	// Define y direction (if velocity > 0, yDir = 1, if velocity < 0, yDir = -1)
	int yDir = (cell.velocity.y > 0.f) ? 1 : -1;

	// Update y velocity
	int ySteps = static_cast<int>(std::abs(cell.velocity.y));

	for (int i = 0; i < ySteps; ++i)
	{

		if (tryMove(x, y, x, y + yDir))
		{
			// Update the y-coordinate to reflect the attempted move down
			y += yDir;
		}

		else
		{
			// Reset velocity if the cell can't move down
			cell.velocity.y = 0.f;	
			break;
		}
	}

	// Define y direction (if velocity > 0, yDir = 1, if velocity < 0, yDir = -1)
	int xDir = (cell.velocity.x > 0.f) ? 1 : -1;

	// Update x Velocity
	int xSteps = static_cast<int>(std::abs(cell.velocity.x));

	for (int i = 0; i < xSteps; ++i)
	{
		if (tryMove(x, y, x + xDir, y))
		{
			// Update the y-coordinate to reflect the attempted move down
			x += xDir;
		}

		else
		{
			// Reset velocity if the cell can't move down
			cell.velocity.x = 0.f;
			break;
		}
	}

	// Velocity damping for particles
	cell.velocity *= 0.92f;

	// Paticle behaviours
	switch (properties.behaviour)
	{
		// Update fire first
		case BehaviourType::Fire:
			updateFire(x, y);	
			break;

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
	if (tryMove(x, y, x, y + 1))
		return;

	// Try to move down-left
	if (tryMove(x, y, x - 1, y + 1))
		return;

	// Try to move down-right
	if (tryMove(x, y, x + 1, y + 1))
		return;

	// If none of the moves were possible, mark the cell as updated for this frame
	getCellRef(x, y).updateFrame = m_currentFrame;
}

void World::updateLiquid(int x, int y)
{
	// Try to move down
	if (tryMove(x, y, x, y + 1))
		return;

	// Try to move left
	if (tryMove(x, y, x - 1, y))
		return;

	// Try to move right
	if (tryMove(x, y, x + 1, y))
		return;

	// Try to move down-left
	if (tryMove(x, y, x - 1, y + 1))
		return;

	// Try to move down-right
	if (tryMove(x, y, x + 1, y + 1))
		return;

	// Randomise when in contact with ground
	randomiseParticleDistribution(x, y);

	// If none of the moves were possible, mark the cell as updated for this frame
	getCellRef(x, y).updateFrame = m_currentFrame;
}

void World::updateGas(int x, int y)
{
	// Try to move up
	if (tryMove(x, y, x, y - 1))
		return;

	// Try to move up-left
	if (tryMove(x, y, x - 1, y - 1))
		return;

	// Try to move up-right
	if (tryMove(x, y, x + 1, y - 1))
		return;

	randomiseParticleDistribution(x, y);

	// If none of the moves were possible, mark the cell as updated for this frame
	getCellRef(x, y).updateFrame = m_currentFrame;
}

void World::updateFire(int x, int y)
{
	Cell& cell = getCellRef(x, y);

	// Spread fire to adjacent cells
	for (int dy = -1; dy <= 1; ++dy)
	{
		for (int dx = -1; dx <= 1; ++dx)
		{
			if (dx == 0 && dy == 0)
				continue;

			int nx = x + dx;
			int ny = y + dy;

			if (!inBounds(nx, ny))
				continue;

			MaterialType neighborMat = getCellRef(nx, ny).material;

			auto& neighborProps = g_materials[static_cast<int>(neighborMat)];

			if (neighborProps.flammable && rand() % 100 < 10) // 10% chance to ignite nearby materials (for natural look)
			{
				setCell(nx, ny, MaterialType::Fire);
				getCellRef(nx, ny).lifeTime = 5; // Fire lasts for 5 frames
			}
		}
	}

	// Decrease lifetime
	if (cell.lifeTime > 0)
	{
		cell.lifeTime--;
	}

	// Turn into smoke or empty when lifetime is over
	if (cell.lifeTime == 0)
	{
		setCell(x, y, MaterialType::Smoke);
		getCellRef(x, y).lifeTime = 10; // Smoke lasts for 10 frames
	}

	cell.updateFrame = m_currentFrame;
}

void World::draw(sf::RenderTarget& target, sf::Vector2f offset) const
{
	if (m_isDirty)
	{
		const_cast<World*>(this)->updateMesh();
	}

	sf::RenderStates states;
	states.transform.translate(offset);

	target.draw(m_vertices, states);
}

// Helper function for painting cells with a brush of a given size and material type
void World::paintCircle(int cx, int cy, int radius, MaterialType type)
{
	for (int dy = -radius; dy <= radius; ++dy)
	{
		for (int dx = -radius; dx <= radius; ++dx)
		{
			if ((dx * dx) + (dy * dy) > radius * radius)
				continue;

			if (rand() % 100 < 9) // Randomly skip some cells for a more natural look
			{
				setCell(cx + dx, cy + dy, type);
			}
		}
	}

	m_isDirty = true;
}

void World::updateMesh()
{
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			int index = (x + y * m_width) * 6;

			sf::Vertex* tri = &m_vertices[index];

			MaterialType matType = getCellRef(x, y).material;

			sf::Color colour = sf::Color::Transparent;

			if (matType != MaterialType::Empty)
			{
				colour = g_materials[static_cast<int>(matType)].colour;
			}

			for (int i = 0; i < 6; ++i)
			{
				tri[i].color = colour;
			}
		}
	}

	m_isDirty = false;
}

void World::explodeParticles(int cx, int cy, int radius)
{
	for (int y = cy - radius; y <= cy + radius; ++y)
	{
		for (int x = cx - radius; x <= cx + radius; ++x)
		{
			if (!inBounds(x, y))
			{
				continue;
			}

			float dx = static_cast<float>(x - cx);
			float dy = static_cast<float>(y - cy);

			float distSq = (dx * dx) + (dy * dy);

			if (distSq > (radius * radius))
			{
				continue;
			}

			float dist = std::sqrtf(distSq);

			if (dist == 0.f)
				dist = 0.1f;

			// Normalise direction
			sf::Vector2f dir(dx / dist, dy / dist);

			float force = (radius - dist) / radius;

			// Apply velocity
			Cell& cell = getCellRef(x, y);

			if (cell.material == MaterialType::Empty)
				continue;

			MaterialType mat = cell.material;

			auto& props = g_materials[static_cast<int>(cell.material)];

			// Add fire property to projectile
			if (props.flammable && rand() % 100 < 40)
			{
				setCell(x, y, MaterialType::Fire);

				getCellRef(x, y).lifeTime = 10;
			}

			// Apply velocity strength to particles based on the material's explosion resistance
			float strength = force * 4.f / props.explosionResistance;

			// Calculate velocity and apply damping
			cell.velocity += dir * strength;

			//printf("particle velocity = { %f, %f }\n", cell.velocity.x, cell.velocity.y);
		}
	}
}

void World::randomiseParticleDistribution(int x, int y)
{
	bool leftFirst = rand() % 2 == 0;

	if (leftFirst)
	{
		if (tryMove(x, y, x - 1, y))
			return;

		if (tryMove(x, y, x + 1, y))
			return;
	}
	else
	{
		if (tryMove(x, y, x + 1, y))
			return;

		if (tryMove(x, y, x - 1, y))
			return;
	}
}
