#include "game.h"

// Constructor to initialise the game
Game::Game():
	m_window(sf::VideoMode({ RW_WIDTH, RW_HEIGHT }), "OpenGL Sand Simulation"),
	m_fpsText(m_font),
	m_player({100.f, 100.f})
{
	m_room_manager.addRoom(
		RW_WIDTH / CELL_SIZE,
		RW_HEIGHT / CELL_SIZE,
		CELL_SIZE,
		TILE_SIZE,
		sf::Vector2i({ 0, 0 }),
		"ROOM 1");

	m_room_manager.addRoom(RW_WIDTH / CELL_SIZE,
		RW_HEIGHT / CELL_SIZE,
		CELL_SIZE,
		TILE_SIZE,
		sf::Vector2i({ 1, 0 }),
		"ROOM 2");

	// Cap fps
	//m_window.setFramerateLimit( 120 );

	//m_world.setTileMap(&m_tilemap); // Set the tile map for the world to enable particle collision detection

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
	for (int x = 0; x < currentRoom().getTileMap().getWidth(); ++x)
	{
		currentRoom().getTileMap().setTile(x, currentRoom().getTileMap().getHeight() - 2, TileType::Solid); // Set the bottom row as solid tiles
	}

	// Initialise views
	m_gameView = m_window.getDefaultView();

	m_overviewView = m_window.getDefaultView();
	m_overviewView.zoom(3.0f);
}

Room& Game::currentRoom()
{
	return m_room_manager.getCurrentRoom();
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

		// Mouse click input
		if (event->is<sf::Event::MouseButtonPressed>())
		{
			auto mouseEvent = event->getIf <sf::Event::MouseButtonPressed>();

			if (m_editorState == EditorState::Gameplay)
			{
				if (mouseEvent->button == sf::Mouse::Button::Left)
				{
					fireProjectile();
				}
			}
			if (m_editorState == EditorState::RoomOverview)
			{
				if (mouseEvent->button == sf::Mouse::Button::Left)
				{
					handleRoomOverviewClick(mouseEvent->position);
				}
			}
		}

		// Mouse wheel scroll
		if (event->is<sf::Event::MouseWheelScrolled>())
		{
			auto scroll = event->getIf<sf::Event::MouseWheelScrolled>();

			m_brushSize += static_cast<int>(scroll->delta);

			if (m_brushSize < 1) m_brushSize = 1;
			if (m_brushSize > 20) m_brushSize = 20;
		}

		// Keyboard Input
		if (event->is<sf::Event::KeyPressed>())
		{
			auto keyEvent = event->getIf<sf::Event::KeyPressed>();

			// Close the window if the Escape key is pressed
			if (keyEvent->code == sf::Keyboard::Key::Escape)
			{
				m_window.close();
			}

			if (keyEvent->code == sf::Keyboard::Key::F1)
				m_editorState = EditorState::Gameplay;

			if (keyEvent->code == sf::Keyboard::Key::F2)
				m_editorState = EditorState::WorldEditor;

			if (keyEvent->code == sf::Keyboard::Key::F3)
					m_editorState = EditorState::RoomOverview;

			if (keyEvent->code == sf::Keyboard::Key::Tab)
				m_roomOverviewMode = !m_roomOverviewMode;

			if (m_editorState == EditorState::WorldEditor)
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

				case sf::Keyboard::Key::Q:
					m_selectedTileType = TileType::Solid;
					break;

				case sf::Keyboard::Key::E:
					m_selectedTileType = TileType::Spike;
					break;
				}
			}
		}
	}
}

void Game::update(float dt)
{
	// Store player position
	sf::Vector2f playerPos = m_player.position;

	if (playerPos.x < 0)
	{
		switchRoom({ -1, 0 });
	}
	else if (playerPos.x > RW_WIDTH)
	{
		switchRoom({ 1, 0 });
	}

	m_playerController.update(m_player, currentRoom().getTileMap(), dt);
	currentRoom().update();

	sf::Vector2i mouse = sf::Mouse::getPosition(m_window);

	int xCell = mouse.x / CELL_SIZE;
	int yCell = mouse.y / CELL_SIZE;

	int xTile = mouse.x / TILE_SIZE;
	int yTile = mouse.y / TILE_SIZE;

	if (m_editorState == EditorState::Gameplay)
	{
		for (auto& projectile : m_projectiles)
		{
			projectile.update(dt);

			// Tile collision detection with projectiles
			int tileX = static_cast<int>(projectile.position.x) / TILE_SIZE;
			int tileY = static_cast<int>(projectile.position.y) / TILE_SIZE;

			// Despawn projectile on collision with a solid tile
			if (currentRoom().getTileMap().isSolid(tileX, tileY))
			{
				projectile.isAlive = false;
			}

			// Material collision detection with projectiles
			int cellX = static_cast<int>(projectile.position.x) / CELL_SIZE;
			int cellY = static_cast<int>(projectile.position.y) / CELL_SIZE;
			int explosionRadius = 12;

			MaterialType mat = currentRoom().getWorld().getCell(cellX, cellY);

			if (mat != MaterialType::Empty)
			{
				currentRoom().getWorld().explode(cellX, cellY, explosionRadius);

				projectile.isAlive = false;

				printf("projectile hit material\n");
			}
		}

		// Erase projectiles at end of lifetime
		m_projectiles.erase(std::remove_if(m_projectiles.begin(), m_projectiles.end(), [](const Projectile& p)
			{
				return !p.isAlive;
			}),
			m_projectiles.end());
	}

	// If the editor mode is active, allow editing the tile map with the left and right mouse buttons
	if (m_editorState == EditorState::WorldEditor)
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		{
			currentRoom().getTileMap().setTile(xTile, yTile, m_selectedTileType);
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
		{
			currentRoom().getTileMap().setTile(xTile, yTile, TileType::Empty);
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle))
		{
			currentRoom().getWorld().paintCircle(xCell, yCell, m_brushSize, m_selectedMaterial);
		}
	}

	if (m_editorState == EditorState::RoomOverview)
	{
		float cameraSpeed = 500.f * dt;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
		{
			m_roomOverviewCameraPosition.x -= cameraSpeed;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
		{
			m_roomOverviewCameraPosition.x += cameraSpeed;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
		{
			m_roomOverviewCameraPosition.y -= cameraSpeed;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
		{
			m_roomOverviewCameraPosition.y += cameraSpeed;
		}

		m_overviewView.setCenter(m_roomOverviewCameraPosition);

		return;
	}
}

void Game::render()
{
	m_window.clear(BACKGROUND_COLOR);

	if (m_editorState == EditorState::Gameplay)
	{
		for (const auto& projectile : m_projectiles)
		{
			projectile.draw(m_window);
		}
	}

	// Draw grid lines in editor mode
	if (m_editorState == EditorState::WorldEditor)
	{
		sf::VertexArray gridLines(sf::PrimitiveType::Lines);

		// Vertical lines
		for (int x = 0; x <= RW_WIDTH / TILE_SIZE; ++x)
		{
			gridLines.append(sf::Vertex(sf::Vector2f(static_cast<float>(x * TILE_SIZE), 0.f), sf::Color(255, 255, 255, 50)));
			gridLines.append(sf::Vertex(sf::Vector2f(static_cast<float>(x * TILE_SIZE), static_cast<float>(RW_HEIGHT)), sf::Color(255, 255, 255, 50)));
		}

		// Horizontal lines
		for (int y = 0; y <= RW_HEIGHT / TILE_SIZE; ++y)
		{
			gridLines.append(sf::Vertex(sf::Vector2f(0.f, static_cast<float>(y * TILE_SIZE)), sf::Color(255, 255, 255, 50)));
			gridLines.append(sf::Vertex(sf::Vector2f(static_cast<float>(RW_WIDTH), static_cast<float>(y * TILE_SIZE)), sf::Color(255, 255, 255, 50)));
		}

		m_window.draw(gridLines);
		
		// Brush stuff
		sf::CircleShape brush;

		brush.setRadius(static_cast<float>(m_brushSize * CELL_SIZE));
		brush.setFillColor(sf::Color::Transparent);
		brush.setOutlineColor(sf::Color::White);
		brush.setOutlineThickness(2.f);

		auto mousePos = sf::Mouse::getPosition(m_window);

		brush.setPosition(sf::Vector2f(static_cast<float>(mousePos.x) - static_cast<float>(m_brushSize * CELL_SIZE), static_cast<float>(mousePos.y) - static_cast<float>(m_brushSize * CELL_SIZE)));

		m_window.draw(brush);
		Game::drawUI();
		Game::drawTileHotbar();
	}

	if (m_editorState == EditorState::RoomOverview)
	{
		m_overviewView.setCenter(m_roomOverviewCameraPosition);
		m_window.setView(m_overviewView);

		drawRoomOverview();
	}
	else
	{
		m_window.setView(m_gameView);

		currentRoom().draw(m_window);
		m_player.draw(m_window);
	}

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

	for (int i = 0; i < 2; ++i)
	{
		sf::RectangleShape box;

		box.setSize({ static_cast<float>(size), static_cast<float>(size) });
		box.setPosition({ 40.f + i * (size + 5), size + 40.f });

		if (i == 0) box.setFillColor(sf::Color::White);   // Solid
		if (i == 1) box.setFillColor(sf::Color::Red);     // Spike

		if (static_cast<int>(m_selectedTileType) == i)
		{
			box.setOutlineThickness(3.f);
			box.setOutlineColor(sf::Color::Green);
		}

		m_window.draw(box);
	}
}

void Game::switchRoom(sf::Vector2i direction)
{
	Room& room = currentRoom();

	sf::Vector2i currentGrid = room.getGridPosition();
	sf::Vector2i targetGrid = { currentGrid.x + direction.x, currentGrid.y + direction.y };

	int roomIndex = m_room_manager.findRoomAtGridPosition(targetGrid);

	if (roomIndex == -1)
		return;

	m_room_manager.setCurrentRoom(roomIndex);

	sf::Vector2f pos = m_player.position;

	if (direction.x > 0)
		pos.x = 50.f;

	else if (direction.x < 0)
		pos.x = RW_WIDTH - 50.f;

	m_player.position = pos;
}

void Game::drawRoomOverview()
{
	const float ROOM_BOX_SIZE = 120.f;
	const float SPACING = 20.f;

	for (int i = 0; i < m_room_manager.getRoomCount(); ++i)
	{
		Room& room = m_room_manager.getRoom(i);

		sf::Vector2i grid = room.getGridPosition();

		sf::Vector2f offset(grid.x * RW_WIDTH, grid.y * RW_HEIGHT);

		room.draw(m_window, offset);

		sf::RectangleShape box;

		box.setSize({ ROOM_BOX_SIZE, ROOM_BOX_SIZE });

		float x = 200.f + grid.x * (ROOM_BOX_SIZE + SPACING);
		float y = 200.f + grid.y * (ROOM_BOX_SIZE + SPACING);

		box.setPosition({ x, y });

		// Highlight current room
		if (&room == &currentRoom())
		{
			box.setFillColor(sf::Color(100, 200, 255));
		}
		else
		{
			box.setFillColor(sf::Color(60, 60, 60));
		}

		box.setOutlineThickness(2.f);
		box.setOutlineColor(sf::Color::White);
	}
}

void Game::handleRoomOverviewClick(sf::Vector2i mousePos)
{
	const float ROOM_BOX_SIZE = 120.f;
	const float SPACING = 20.f;

	for (int i = 0; i < m_room_manager.getRoomCount(); ++i)
	{
		Room& room = m_room_manager.getRoom(i);

		sf::Vector2i grid = room.getGridPosition();

		float x = 200 + grid.x * (ROOM_BOX_SIZE + SPACING);

		float y = 200 + grid.y * (ROOM_BOX_SIZE + SPACING);

		sf::FloatRect bounds({ x, y }, { ROOM_BOX_SIZE, ROOM_BOX_SIZE });

		if (bounds.contains(sf::Vector2f(mousePos)))
		{
			m_room_manager.setCurrentRoom(i);
			break;
		}
	}
}

void Game::fireProjectile()
{
	sf::Vector2f dir = { 0.f, 0.f };

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
		dir.y = -1.f;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
		dir.y = 1.f;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
		dir.x = -1.f;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
		dir.x = 1.f;

	// Default direction if no input
	if (dir == sf::Vector2f(0.f, 0.f))
		dir.x = 1.f;

	// Normalise diagonal movement
	float length = std::sqrt((dir.x * dir.x) + (dir.y * dir.y));

	dir /= length;

	Projectile projectile;

	projectile.position = m_player.position;
	projectile.velocity = dir * 600.f;

	m_projectiles.push_back(projectile);
}