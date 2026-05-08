#pragma once

#include <SFML/Graphics.hpp>
#include "player.h"
#include "player_controller.h"
#include "room_manager.h"

#include <iostream>

enum class EditorState
{
	Gameplay,
	WorldEditor,
	RoomOverview
};

class Game
{
public:
	Game();									// Constructor
	void run();								// Main game loop
		
private:
	void processEvents();					// Handle user input and window events
	void update(float dt);					// Update game state (player, world, etc.)
	void render();							// Render the game (world, player, UI, etc.)
	void drawUI();							// Draw the user interface (FPS, brush size, selected material/tile, etc.)
	void drawTileHotbar();					// Draw the tile hotbar for selecting tile types in editor mode
	void drawRoomOverview();				// Draw the overview for our room layout
	void handleRoomOverviewClick(sf::Vector2i mousePos);

	sf::RenderWindow m_window;				// SFML window for rendering

	MaterialType m_selectedMaterial = MaterialType::Sand;		// Currently selected material for painting in the world
	TileType m_selectedTileType = TileType::Solid;				// Currently selected tile type for editing the tile map

	sf::Font m_font;						// Font for rendering text
	sf::Text m_fpsText;						// Text object for displaying FPS
	const static int CHARACTER_SIZE = 20;	// Size of the text for displaying FPS
	float m_fps = 0.f;						// Current frames per second

	RoomManager m_room_manager;				// Room manager for loading and switiching between rooms
	Room& currentRoom();					// Current Room

	Player m_player;						// The player character
	PlayerController m_playerController;	// Controller for handling player input and movement

	sf::Clock m_clock;						// SFML clock for measuring frame time

	bool m_isRunning = true;				// Flag to indicate if the game is running

	int m_brushSize = 4;					// Size of the brush for painting materials in the world

	const static int RW_WIDTH = 1280;		// Width of the render window
	const static int RW_HEIGHT = 720;		// Height of the render window
	const static int CELL_SIZE = 3;			// Size of each cell in the world
	const static int TILE_SIZE = 32;		// Size of each tile in the tile map

	const sf::Color BACKGROUND_COLOR = sf::Color(0, 20, 85);	// Background color of the window

	EditorState m_editorState = EditorState::Gameplay;	// Flag to check which editor state we are in

	void switchRoom(sf::Vector2i direction);
};