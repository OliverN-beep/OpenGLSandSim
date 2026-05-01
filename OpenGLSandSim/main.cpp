#include "world.h"
#include <iostream>
#include <chrono>

#include "player.h"
#include "player_controller.h"

#include "tilemap.h"

Player player({100.f, 100.f}); // Initialise player at position (100, 100)
PlayerController playerController; // Create a player controller instance

int main()
{
	const int RW_WIDTH = 1200;
	const int RW_HEIGHT = 720;

	const int CELL_SIZE = 6;
	const int COLUMNS = RW_WIDTH / CELL_SIZE;
	const int ROWS = RW_HEIGHT / CELL_SIZE;

	const int TILE_SIZE = 32; // Size of each tile in pixels
	const int TILE_COLUMNS = RW_WIDTH / TILE_SIZE;
	const int TILE_ROWS = RW_HEIGHT / TILE_SIZE;

	const sf::Color BACKGROUND_COLOR = sf::Color(0, 10, 55);

	sf::RenderWindow window(sf::VideoMode({ RW_WIDTH, RW_HEIGHT }), "OpenGL Sand Simulation");
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(120);

	// Instantiate the World with the specified number of columns, rows, and cell size
	World world(COLUMNS, ROWS, CELL_SIZE);

	// Set up font to display text
	sf::Font font;
	if (!font.openFromFile("fonts/monospace_medium.ttf"))
	{
		std::cerr << "Could not load font" << std::endl;
		return -1;
	}

	// Set up text to display FPS
	std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::time_point end;

	float fps = 0.0f;

	sf::Text fpsText(font);
	fpsText.setFont(font);
	fpsText.setCharacterSize(20);
	fpsText.setFillColor(sf::Color::White);
	fpsText.setPosition({RW_WIDTH - 150, 50});

	// SFML clock used for frame delta time (declare to shadow the C runtime clock() function)
	sf::Clock clock;

	// Set up the tile map with the specified number of columns, rows, and tile size
	TileMap tileMap(TILE_COLUMNS, TILE_ROWS, TILE_SIZE);

	// Build simple test room with tile map floor
	for (int x = 0; x < TILE_COLUMNS; ++x)
	{
		tileMap.setTile(x, TILE_ROWS - 1, TileType::Solid); // Set the bottom row as solid tiles
	}

	// ----------- Start the game loop -----------
	while (window.isOpen())
	{
		// Handle events
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}

			auto mousePos = sf::Mouse::getPosition(window);

			int x = mousePos.x / CELL_SIZE;
			int y = mousePos.y / CELL_SIZE;

			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
			{
				world.setCell(x, y, MaterialType::Water);
			}

			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
			{
				world.setCell(x, y, MaterialType::Sand);
			}
		}

		// Measure the start time
		start = std::chrono::high_resolution_clock::now();
		
		// Update player state based on input and world state
		float dt = clock.restart().asSeconds();
		playerController.update(player, tileMap, dt);

		// Update the world state
		world.update();

		window.clear(BACKGROUND_COLOR);

		tileMap.draw(window);
		world.draw(window);
		player.draw(window);
		window.draw(fpsText);

		window.display();

		// Measure the end time
		end = std::chrono::high_resolution_clock::now();

		fps = 1.0f / std::chrono::duration<float>(end - start).count();			// Calculate FPS based on the time taken for one frame
		fpsText.setString("FPS: " + std::to_string(static_cast<int>(fps)));		// Update the FPS text
	}

	// Loop end: release resources...
	
}
