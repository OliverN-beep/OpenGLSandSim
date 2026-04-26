#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include "materials.h"

class World												// Class representing the simulation world
{
public:
	World(int width, int height, int cellSize);			// Constructor to initialize the world with given dimensions and cell size

	void update();										// Update the world state (e.g., sand falling)
	void draw(sf::RenderWindow& window);				// Draw the world to the given SFML window
	void setCell(int x, int y, MaterialType matType);	// Set the material type of a specific cell
	MaterialType getCell(int x, int y) const;			// Get the material type of a specific cell
	
private:
	int m_width;										// Width of the world in cells
	int m_height;										// Height of the world in cells
	int m_cellSize;										// Size of each cell in pixels

	std::vector<MaterialType> cells;					// 1D array representing the world grid

	int index(int x, int y) const;						// Convert 2D coordinates to 1D index
	bool inBounds(int x, int y) const;					// Check if the given coordinates are within the world bounds

	void updateSand(int x, int y);						// Update the state of a sand cell at the given coordinates
	void updateWater(int x, int y);						// Update the state of a water cell at the given coordinates
	void updateOil(int x, int y);						// Update the state of an oil cell at the given coordinates
	void updateFire(int x, int y);						// Update the state of a fire cell at the given coordinates
	void updateSmoke(int x, int y);						// Update the state of a smoke cell at the given coordinates
};