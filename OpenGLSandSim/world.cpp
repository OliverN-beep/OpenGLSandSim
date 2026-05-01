#include "world.h"

World::World(int width, int height, int cellSize)
	: m_width(width), m_height(height), m_cellSize(cellSize), cells(width * height, MaterialType::Empty)
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

MaterialType World::getCell(int x, int y) const
{
	if (inBounds(x, y))
	{
		return cells[index(x, y)];
	}
	return MaterialType::Empty;
}

void World::setCell(int x, int y, MaterialType matType)
{
	if (inBounds(x, y))
	{
		cells[index(x, y)] = matType;
	}
}

// Update the state of a sand cell at the given coordinates
void World::updateSand(int x, int y)
{
	// Check if the current cell is sand
	if (getCell(x, y) == MaterialType::Sand)
	{
		// Check if the cell below is empty, if so, move the sand down
		if (inBounds(x, y + 1) && getCell(x, y + 1) == MaterialType::Empty)
		{
			// Move the sand down
			setCell(x, y + 1, MaterialType::Sand);
			setCell(x, y, MaterialType::Empty);
		}
		// If the cell below is not empty, check the diagonal cells
		else if (inBounds(x - 1, y + 1) && getCell(x - 1, y + 1) == MaterialType::Empty)
		{
			// Move the sand to the left diagonal cell
			setCell(x - 1, y + 1, MaterialType::Sand);
			setCell(x, y, MaterialType::Empty);
		}
		// If the left diagonal cell is not empty, check the right diagonal cell
		else if (inBounds(x + 1, y + 1) && getCell(x + 1, y + 1) == MaterialType::Empty)
		{
			// Move the sand to the right diagonal cell
			setCell(x + 1, y + 1, MaterialType::Sand);
			setCell(x, y, MaterialType::Empty);
		}
	}
}

// Same logic as sand, but water can also move sideways if blocked
void World::updateWater(int x, int y)
{
	if (getCell(x, y) == MaterialType::Water)
	{
		if (inBounds(x, y + 1) && getCell(x, y + 1) == MaterialType::Empty)
		{
			setCell(x, y + 1, MaterialType::Water);
			setCell(x, y, MaterialType::Empty);
		}
		else if (inBounds(x - 1, y + 1) && getCell(x - 1, y + 1) == MaterialType::Empty)
		{
			setCell(x - 1, y + 1, MaterialType::Water);
			setCell(x, y, MaterialType::Empty);
		}
		else if (inBounds(x + 1, y + 1) && getCell(x + 1, y + 1) == MaterialType::Empty)
		{
			setCell(x + 1, y + 1, MaterialType::Water);
			setCell(x, y, MaterialType::Empty);
		}

		// If the cell below and diagonal cells are not empty, try to move left or right
		else if (inBounds(x - 1, y) && getCell(x - 1, y) == MaterialType::Empty)
		{
			setCell(x - 1, y, MaterialType::Water);
			setCell(x, y, MaterialType::Empty);
		}
		else if (inBounds(x + 1, y) && getCell(x + 1, y) == MaterialType::Empty)
		{
			setCell(x + 1, y, MaterialType::Water);
			setCell(x, y, MaterialType::Empty);
		}
	}
}

void World::updateOil(int x, int y)
{
	// Placeholder for oil update logic
}

void World::updateFire(int x, int y)
{
	// Placeholder for fire update logic
}

// Same logic as sand, but smoke rises instead of falling
void World::updateSmoke(int x, int y)
{
	if (getCell(x, y) == MaterialType::Smoke)
	{
		if (inBounds(x, y - 1) && getCell(x, y - 1) == MaterialType::Empty)
		{
			setCell(x, y - 1, MaterialType::Smoke);
			setCell(x, y, MaterialType::Empty);
		}
		else if (inBounds(x - 1, y - 1) && getCell(x - 1, y - 1) == MaterialType::Empty)
		{
			setCell(x - 1, y - 1, MaterialType::Smoke);
			setCell(x, y, MaterialType::Empty);
		}
		else if (inBounds(x + 1, y - 1) && getCell(x + 1, y - 1) == MaterialType::Empty)
		{
			setCell(x + 1, y - 1, MaterialType::Smoke);
			setCell(x, y, MaterialType::Empty);
		}
	}
}

void World::updateSnow(int x, int y)
{
	// Placeholder for snow update logic
}

void World::updateSalt(int x, int y)
{
	// Placeholder for salt update logic
}

void World::update()
{
	// Falling materials
	for (int y = m_height - 1; y >= 0; --y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			updateSand(x, y);
			updateWater(x, y);
			updateOil(x, y);
			updateSnow(x, y);
			updateSalt(x, y);
		}
	}

	// Rising materials
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			updateSmoke(x, y);
		}
	}

	// Update other materials last if needed
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			updateFire(x, y);
		}
	}
}

void World::draw(sf::RenderWindow& window) const
{
	// Create a rectangle shape to represent each cell
	sf::RectangleShape rect({ (float)m_cellSize, (float)m_cellSize });

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

			rect.setPosition({ (float)(x * m_cellSize), (float)(y * m_cellSize) });
			window.draw(rect);
		}
	}
}