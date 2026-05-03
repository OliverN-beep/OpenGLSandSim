#include "game.h"

// Constructor to initialise the game
Game::Game()
	: m_window(sf::VideoMode({ RW_WIDTH, RW_HEIGHT }), "OpenGL Sand Simulation"),
	  m_fpsText(m_font),	// Text object for displaying FPS
	  m_world(RW_WIDTH / CELL_SIZE, RW_HEIGHT / CELL_SIZE, CELL_SIZE), // World with cell size of 6 pixels
	  m_tilemap(RW_WIDTH / TILE_SIZE, RW_HEIGHT / TILE_SIZE, TILE_SIZE), // TileMap with tile size of 32 pixels
	  m_player({100.f, 100.f}) // Initialise player at position (100, 100)
{
	m_window.setFramerateLimit(60);

	m_world.setTileMap(&m_tilemap); // Set the tile map for the world to enable particle collision detection

	// Load font for displaying text
	if (!m_font.openFromFile("fonts/monospace_medium.ttf"))
	{
		std::cerr << "Could not load font" << std::endl;
	}

	m_fpsText.setFont(m_font);
	m_fpsText.setCharacterSize(CHARACTER_SIZE);
	m_fpsText.setFillColor(sf::Color::White);
	m_fpsText.setPosition({ RW_WIDTH - 150.f, 50.f });

	// Build simple test room with tile map floor
	for (int x = 0; x < m_tilemap.getWidth(); ++x)
	{
		m_tilemap.setTile(x, m_tilemap.getHeight() - 1, TileType::Solid); // Set the bottom row as solid tiles
	}
}

void Game::run()
{
	while (m_window.isOpen())
	{
		processEvents();

		float dt = m_clock.restart().asSeconds();
		m_fps = dt;
		update(dt);
		render();
	}
}

void Game::processEvents()
{
	while (const std::optional<sf::Event> event = m_window.pollEvent())
	{
		if (event->is<sf::Event::Closed>())
		{
			m_window.close();
		}

		if (event->is<sf::Event::MouseWheelScrolled>())
		{
			auto scroll = event->getIf<sf::Event::MouseWheelScrolled>();

			m_brushSize += (int)scroll->delta;

			if (m_brushSize < 1) m_brushSize = 1;
			if (m_brushSize > 20) m_brushSize = 20;
		}
	}
}

void Game::update(float dt)
{
	m_playerController.update(m_player, m_tilemap, dt);
	m_world.update();

	sf::Vector2i mouse = sf::Mouse::getPosition(m_window);

	int x = mouse.x / CELL_SIZE;
	int y = mouse.y / CELL_SIZE;

	// Draw materials in the world based on mouse input
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
	{
		m_world.paintCircle(x, y, m_brushSize, MaterialType::Salt);
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
	{
		m_world.paintCircle(x, y, m_brushSize, MaterialType::Water);
	}
}

void Game::render()
{
	m_window.clear(BACKGROUND_COLOR);

	m_world.draw(m_window);
	m_tilemap.draw(m_window);
	m_player.draw(m_window);

	// Brush
	sf::CircleShape brush;
	brush.setRadius(static_cast<float>(m_brushSize * CELL_SIZE));
	brush.setFillColor(sf::Color::Transparent);
	brush.setOutlineColor(sf::Color::White);
	brush.setOutlineThickness(2.f);
	brush.setPosition(sf::Vector2f(sf::Mouse::getPosition(m_window)) - sf::Vector2f(static_cast<float>(m_brushSize * CELL_SIZE), static_cast<float>(m_brushSize * CELL_SIZE)));

	auto mousePos = sf::Mouse::getPosition(m_window);
	brush.setPosition(sf::Vector2f(static_cast<float>(mousePos.x) - static_cast<float>(m_brushSize * CELL_SIZE), static_cast<float>(mousePos.y) - static_cast<float>(m_brushSize * CELL_SIZE)));
	m_window.draw(brush);

	// Calculate and display FPS
	float fps = 1.f / m_fps;
	m_fpsText.setString("FPS: " + std::to_string(static_cast<int>(fps)));
	m_window.draw(m_fpsText);

	m_window.display();
}