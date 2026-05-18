#pragma once

#include <vector>
#include "materials.h"

// Forward declaration of TileMap class
class TileMap;

struct Cell
{
	MaterialType material = MaterialType::Empty;						// Material type of the cell
	unsigned int updateFrame = 0;										// Frame number when the cell was last updated

	uint8_t lifeTime = 0;												// Lifetime of the cell (fire, smoke, etc.)
	uint8_t state = 0;													// Additional state information for the cell (fire spreading, wet material, etc.)

	sf::Vector2f velocity = { 0.f, 0.f };								// Velocity of the cell
};

class World																// Class representing the simulation world
{	
public:
	void setTileMap(TileMap* tileMap);									// Set the tile map for the world (used for particle collision detection)
		
	World(int width, int height, int cellSize);							// Constructor to initialize the world with given dimensions and cell size

	sf::VertexArray m_vertices;											// Get the vertex array for rendering every cell in the world at once

	void update();														// Update the world state (e.g., sand falling)
	void draw(sf::RenderWindow& window, sf::Vector2f offset = { 0.f, 0.f }) const;							// Draw the world to the given SFML window

	MaterialType getCell(int x, int y) const;							// Get the material type of a specific cell
	void setCell(int x, int y, MaterialType matType);					// Set the material type of a specific cell

	void paintCircle(int cx, int cy, int radius, MaterialType type);	// Paint a circle of cells with the given material type

	void updateMesh();													// Update the vertex array for rendering

	void explodeParticles(int cx, int cy, int radius);					// Spawns explosions	

private:
	TileMap* m_tileMap = nullptr;						// Pointer to the tile map for particle collision detection

	int m_width;										// Width of the world in cells
	int m_height;										// Height of the world in cells
	int m_cellSize;										// Size of each cell in pixels

	unsigned int m_currentFrame = 0;					// Current frame number for update tracking

	bool m_isDirty = true;								// Flag to indicate if the world state has changed and needs to be redrawn

	std::vector<Cell> cells;							// 1D array representing the world grid

	int index(int x, int y) const;						// Convert 2D coordinates to 1D index
	bool inBounds(int x, int y) const;					// Check if the given coordinates are within the world bounds
	bool isTileBlocked(int x, int y) const;				// Check if the cell at the given coordinates is blocked (not empty or solid tile)

	Cell& getCellRef(int x, int y);						// Get a reference to the cell at the given coordinates
	const Cell& getCellRef(int x, int y) const;			// Get a const reference to the cell at the given coordinates

	void updateCellBehaviour(int x, int y);				// Update the state of a cell at the given coordinates based on its material type

	void updatePowder(int x, int y);					// Update the state of a powder cell at the given coordinates
	void updateLiquid(int x, int y);					// Update the state of a liquid cell at the given coordinates
	void updateGas(int x, int y);						// Update the state of a gas cell at the given coordinates

	void updateFire(int x, int y);						// Update the state of a fire cell at the given coordinates

	bool canMoveInto(int x1, int y1, int x2, int y2);	// Check if a cell can move from (x1, y1) to (x2, y2) based on material properties and tile map
	bool tryMove(int x, int y, int newX, int newY);		// Attempt to move a cell from (x, y) to (newX, newY) if the destination is empty
	bool isEmpty(int x, int y) const;					// Check if the cell at the given coordinates is empty

	void randomiseParticleDistribution(int x, int y);	// Helper function to randomise distribution of particles (mainly for liquids and gasses)
};