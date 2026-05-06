#include "game.h"

// Constructor to initialise the game
Game::Game():
	m_window(sf::VideoMode({ RW_WIDTH, RW_HEIGHT }), "OpenGL Sand Simulation"),
	m_fpsText(m_font),
	m_world(RW_WIDTH / CELL_SIZE, RW_HEIGHT / CELL_SIZE, CELL_SIZE),
	m_tilemap(RW_WIDTH / TILE_SIZE, RW_HEIGHT / TILE_SIZE, TILE_SIZE),
	m_player({100.f, 100.f}),
	m_selectedMaterial(MaterialType::Sand),
	m_selectedTileType(TileType::Solid)
{
	// Cap fps
	m_window.setFramerateLimit( 120 );

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

			m_brushSize += static_cast<int>(scroll->delta);

			if (m_brushSize < 1) m_brushSize = 1;
			if (m_brushSize > 20) m_brushSize = 20;
		}

		// Close the window if the Escape key is pressed
		if (event->is<sf::Event::KeyPressed>())
		{
			auto keyEvent = event->getIf<sf::Event::KeyPressed>();
			if (keyEvent->code == sf::Keyboard::Key::Escape)
			{
				m_window.close();
			}
		}

		if (event->is<sf::Event::KeyPressed>())
		{
			auto keyEvent = event->getIf<sf::Event::KeyPressed>();

			if (keyEvent->code == sf::Keyboard::Key::Tab)
			{
				m_editorMode = !m_editorMode;
				printf("Editor mode: %s\n", m_editorMode ? "ON" : "OFF");
			}

			if (!m_editorMode)
			{
				switch (keyEvent->code)
				{
				case sf::Keyboard::Key::Num1:
					m_selectedMaterial = MaterialType::Sand;
					break;

				case sf::Keyboard::Key::Num2:
					m_selectedMaterial = MaterialType::Water;
					break;

				case sf::Keyboard::Key::Num3:
					m_selectedMaterial = MaterialType::Stone;
					break;

				case sf::Keyboard::Key::Num4:
					m_selectedMaterial = MaterialType::Oil;
					break;

				case sf::Keyboard::Key::Num5:
					m_selectedMaterial = MaterialType::Fire;
					break;

				case sf::Keyboard::Key::Num6:
					m_selectedMaterial = MaterialType::Smoke;
					break;

				case sf::Keyboard::Key::Num7:
					m_selectedMaterial = MaterialType::Snow;
					break;

				case sf::Keyboard::Key::Num8:
					m_selectedMaterial = MaterialType::Wood;
					break;

				case sf::Keyboard::Key::Num9:
					m_selectedMaterial = MaterialType::Salt;
					break;
				}
			}

			if (m_editorMode)
			{
				switch (keyEvent->code)
				{
				case sf::Keyboard::Key::Num1:
					m_selectedTileType = TileType::Empty;
					break;

				case sf::Keyboard::Key::Num2:
					m_selectedTileType = TileType::Solid;
					break;

				case sf::Keyboard::Key::Num3:
					m_selectedTileType = TileType::Spike;
					break;
				}
			}
		}
	}
}

void Game::update(float dt)
{
	m_playerController.update(m_player, m_tilemap, dt);
	m_world.update();

	sf::Vector2i mouse = sf::Mouse::getPosition(m_window);

	int xCell = mouse.x / CELL_SIZE;
	int yCell = mouse.y / CELL_SIZE;

	int xTile = mouse.x / TILE_SIZE;
	int yTile = mouse.y / TILE_SIZE;

	// If the editor mode is active, allow editing the tile map with the left and right mouse buttons
	if (m_editorMode && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
	{
		m_tilemap.setTile(xTile, yTile, m_selectedTileType);
	}

	// If the editor mode is not active, allow painting materials in the world with the left mouse button
	if (!m_editorMode && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		{
			m_world.paintCircle(xCell, yCell, m_brushSize, m_selectedMaterial);
		}
	}
}

void Game::render()
{
	m_window.clear(BACKGROUND_COLOR);

	m_world.draw(m_window);
	m_tilemap.draw(m_window);
	m_player.draw(m_window);

	Game::drawUI();
	Game::drawTileHotbar();

	// Draw grid lines in editor mode
	if (m_editorMode)
	{
		sf::VertexArray gridLines(sf::PrimitiveType::Lines);

		// Vertical lines
		for (int x = 0; x <= RW_WIDTH; ++x)
		{
			gridLines.append(sf::Vertex(sf::Vector2f(static_cast<float>(x * TILE_SIZE), 0.f), sf::Color(255, 255, 255, 50)));
			gridLines.append(sf::Vertex(sf::Vector2f(static_cast<float>(x * TILE_SIZE), static_cast<float>(RW_HEIGHT)), sf::Color(255, 255, 255, 50)));
		}

		// Horizontal lines
		for (int y = 0; y <= RW_HEIGHT; ++y)
		{
			gridLines.append(sf::Vertex(sf::Vector2f(0.f, static_cast<float>(y * TILE_SIZE)), sf::Color(255, 255, 255, 50)));
			gridLines.append(sf::Vertex(sf::Vector2f(static_cast<float>(RW_WIDTH), static_cast<float>(y * TILE_SIZE)), sf::Color(255, 255, 255, 50)));
		}

		m_window.draw(gridLines);
	}

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

	// Draw the FPS text on the window
	m_window.draw(m_fpsText);

	// End the current frame and display its contents on screen
	m_window.display();
}

void Game::drawUI()
{
	const int SIZE = 20;
	const int PADDING = 10;

	for (int i = 0; i < static_cast<int>(MaterialType::COUNT); ++i)
	{
		MaterialType matType = static_cast<MaterialType>(i);
		auto& mat = g_materials[i];

		sf::RectangleShape rect(sf::Vector2f(SIZE, SIZE));
		rect.setFillColor(mat.colour);

		int xPos = PADDING + i * (SIZE + PADDING);

		rect.setPosition(sf::Vector2f(static_cast<float>(xPos), static_cast<float>(PADDING)));

		if (matType == m_selectedMaterial)
		{
			rect.setOutlineColor(sf::Color::White);
			rect.setOutlineThickness(2.f);
		}
		else
		{
			rect.setOutlineColor(sf::Color::Transparent);
			rect.setOutlineThickness(0.f);
		}

		m_window.draw(rect);
	}
}

void Game::drawTileHotbar()
{
	const int size = 40;

	for (int i = 0; i < 3; ++i)
	{
		sf::RectangleShape box;

		box.setSize({ static_cast<float>(size), static_cast<float>(size) });
		box.setPosition({ 40.f + i * (size + 5), size + 40.f });

		if (i == 0) box.setFillColor(sf::Color::Black);   // Empty
		if (i == 1) box.setFillColor(sf::Color::White);   // Solid
		if (i == 2) box.setFillColor(sf::Color::Red);     // Spike

		if (static_cast<int>(m_selectedTileType) == i)
		{
			box.setOutlineThickness(3.f);
			box.setOutlineColor(sf::Color::Red);
		}

		m_window.draw(box);
	}
}