#pragma once

#include <SFML/Graphics.hpp>
#include "player.h"
#include "player_controller.h"
#include "world.h"
#include "tilemap.h"

#include <iostream>

class Game
{
public:
	Game();									// Constructor
	void run();								// Main game loop
		
private:
	void processEvents();					// Handle user input and window events
	void update(float dt);					// Update game state (player, world, etc.)
	void render();							// Render the game (world, player, UI, etc.)

	sf::RenderWindow m_window;				// SFML window for rendering
	sf::Font m_font;						// Font for rendering text
	sf::Text m_fpsText;						// Text object for displaying FPS

	const static int CHARACTER_SIZE = 20;	// Size of the text for displaying FPS

	World m_world;							// The simulation world
	TileMap m_tilemap;						// The tile map for the world

	Player m_player;						// The player character
	PlayerController m_playerController;	// Controller for handling player input and movement

	sf::Clock m_clock;						// SFML clock for measuring frame time

	bool m_isRunning = true;				// Flag to indicate if the game is running

	int m_brushSize = 4;					// Size of the brush for painting materials in the world

	float m_fps = 0.f;						// Current frames per second

	const static int RW_WIDTH = 1280;		// Width of the render window
	const static int RW_HEIGHT = 720;		// Height of the render window
	const static int CELL_SIZE = 6;			// Size of each cell in the world
	const static int TILE_SIZE = 32;		// Size of each tile in the tile map

	const sf::Color BACKGROUND_COLOR = sf::Color(0, 10, 55);	// Background color of the window
};