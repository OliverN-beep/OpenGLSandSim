#pragma once

#include "player.h"
#include "player_controller.h"
#include "room_manager.h"
#include "projectile.h"

#include <iostream>

enum class GameState
{
	Gameplay,
	Editor
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
	void drawMaterialUI();					// Draw the user interface (FPS, brush size, selected material/tile, etc.)
	void drawTilePalette();					// Draw the tileset atlas for use in editor mode
	void handleTilePaletteClick(sf::Vector2i mousePos);			// Handles clicking on tile palette
	void switchRoom(sf::Vector2i direction);
	void applyPlayerExplosionKnockback(sf::Vector2f explosionPos, float radius, float force);
	void fireProjectile();					// Fires player projectile
	sf::Vector2f getMouseGamePosition() const;

	sf::RenderWindow m_window;				// SFML window for rendering

	MaterialType m_selectedMaterial = MaterialType::Sand;		// Currently selected material for painting in the world
	TileType m_selectedTileType = TileType::Solid;				// Currently selected tile type for editing the tile map

	sf::Font m_font;						// Font for rendering text
	sf::Text m_fpsText;						// Text object for displaying FPS

	const static int CHARACTER_SIZE = 28;	// Size of the text for displaying FPS
	float m_fps = 0.f;						// Current frames per second

	RoomManager m_room_manager;				// Room manager for loading and switiching between rooms
	Room& currentRoom();					// Current Room

	Player m_player;						// The player character
	PlayerController m_playerController;	// Controller for handling player input and movement

	sf::Clock m_clock;						// SFML clock for measuring frame time

	bool m_isRunning = true;				// Flag to indicate if the game is running

	int m_brushSize = 4;					// Size of the brush for painting materials in the world

	const static int WINDOW_WIDTH = 1280;
	const static int WINDOW_HEIGHT = 720;

	const static int GAME_WIDTH = 320;
	const static int GAME_HEIGHT = 180;

	const static int CELL_SIZE = 1;			// Size of each cell in the world
	const static int TILE_SIZE = 16;		// Size of each tile in the tile map

	const sf::Color BACKGROUND_COLOR = sf::Color(0, 20, 125);	// Background color of the window

	GameState m_editorState = GameState::Gameplay;	// Flag to check which editor state we are in

	sf::View m_gameView;					// View for gameplay

	std::vector<Projectile> m_projectiles;	// Projectile instance

	TileMap* m_tilemap = nullptr;

	bool m_keyHeldLastFrameW = false;
	bool m_keyHeldLastFrameA = false;
	bool m_keyHeldLastFrameS = false;
	bool m_keyHeldLastFrameD = false;
	bool m_keyHeldLastFrameDel = false;

	sf::FloatRect m_tilePaletteBounds;

	sf::RenderTexture m_gameTexture;
};