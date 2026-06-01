#include "game.h"

// Constructor to initialise the game
Game::Game():
	m_window(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "OpenGL Sand Simulation", sf::State::Windowed),
	m_fpsText(m_font),
	m_player({100.f, 100.f}),	// Set initial spawn location
	m_room_manager(GAME_WIDTH / CELL_SIZE, GAME_HEIGHT / CELL_SIZE, CELL_SIZE, TILE_SIZE)
{
	// Cap fps
	m_window.setFramerateLimit( 60 );

	// Set the tile map for the world to enable particle collision detection
	currentRoom().getWorld().setTileMap(&currentRoom().getTileMap());

	// Load font for displaying text
	if (!m_font.openFromFile("fonts/monospace_medium.ttf"))
	{
		std::cerr << "Could not load font" << std::endl;
	}

	m_fpsText.setFont(m_font);
	m_fpsText.setCharacterSize(CHARACTER_SIZE);
	m_fpsText.setFillColor(sf::Color::White);
	m_fpsText.setPosition({ WINDOW_WIDTH - 150, 50 });

	// Load world from JSON on startup (if it exists)
	m_room_manager.loadWorld("world.json");

	// Initialise views
	m_gameView = m_window.getDefaultView();

	// Initialise low resolution render texture
	if (!m_gameTexture.resize({ GAME_WIDTH, GAME_HEIGHT }))
	{
		printf("failed to create render texture\n");
	}

	m_gameTexture.setSmooth(false);
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

			if (m_editorState == GameState::Gameplay)
			{
				if (mouseEvent->button == sf::Mouse::Button::Left)
				{
					fireProjectile();
				}
			}

			if (m_editorState == GameState::Editor)
			{
				if (mouseEvent->button == sf::Mouse::Button::Left)
				{
					sf::Vector2f mousePos = getMouseGamePosition();

					if (m_tilePaletteBounds.contains(mousePos))
					{
						handleTilePaletteClick(sf::Vector2i(mousePos));
					}
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
				m_editorState = GameState::Gameplay;

			if (keyEvent->code == sf::Keyboard::Key::F2)
				m_editorState = GameState::Editor;

			if (m_editorState == GameState::Editor)
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

				if (keyEvent->code == sf::Keyboard::Key::F5)
				{
					m_room_manager.saveWorld("world.json");
					printf("WORLD SAVED\n");
				}

				if (keyEvent->code == sf::Keyboard::Key::F9)
				{
					m_room_manager.loadWorld("world.json");
					printf("WORLD LOADED\n");
				}
			}
		}
	}
}

void Game::update(float dt)
{
	// Store player position
	sf::Vector2f playerPos = m_player.position;

	// Store player bounds for collectable collision detection
	sf::FloatRect playerBounds(playerPos, m_player.size);

	// Handle room switching when player goes out of bounds
	if (playerPos.x < 0)
		switchRoom({ -1, 0 });

	else if (playerPos.x > GAME_WIDTH)
		switchRoom({ 1, 0 });

	if (playerPos.y < 0)
		switchRoom({ 0, -1 });

	else if (playerPos.y > GAME_HEIGHT)
		switchRoom({ 0, 1 });

	currentRoom().update();

	sf::Vector2f mouse = getMouseGamePosition();

	int xCell = static_cast<int>(mouse.x) / CELL_SIZE;
	int yCell = static_cast<int>(mouse.y) / CELL_SIZE;

	int xTile = static_cast<int>(mouse.x) / TILE_SIZE;
	int yTile = static_cast<int>(mouse.y) / TILE_SIZE;

	bool mouseOverPalette = m_tilePaletteBounds.contains(sf::Vector2f(mouse));

	if (m_editorState == GameState::Gameplay)
	{
		// Projectile updates and collision detection
		for (auto& projectile : m_projectiles)
		{
			projectile.update(dt);

			// Tile collision detection with projectiles
			int tileX = static_cast<int>(projectile.position.x) / TILE_SIZE;
			int tileY = static_cast<int>(projectile.position.y) / TILE_SIZE;

			// Prevent collision from running after projectile has exploded
			if (!projectile.isAlive)
				continue;

			// Check projectile collision with SOLID tiles
			if (currentRoom().getTileMap().isSolid(tileX, tileY))
			{
				applyPlayerExplosionKnockback(projectile.position, projectile.explosionRadius, projectile.playerKnockback);

				projectile.isAlive = false;
			}

			// Check projectile collision with SPIKE tiles
			if (currentRoom().getTileMap().isSpike(tileX, tileY))
			{
				// Increased player knockback for spike tiles
				// This is larger due to the player never landing on tiles and thus being further from the explosion radius
				float spikeMultiplier = 1.45f;
				applyPlayerExplosionKnockback(projectile.position, projectile.explosionRadius, projectile.playerKnockback * spikeMultiplier);

				projectile.isAlive = false;
			}

			// Check projectile collision with BOUNCE tiles
			if (currentRoom().getTileMap().isBounce(tileX, tileY))
			{
				// Increased player knockback for bounce tiles
				float bounceMultiplier = 1.55f;
				applyPlayerExplosionKnockback(projectile.position, projectile.explosionRadius, projectile.playerKnockback * bounceMultiplier);

				projectile.isAlive = false;
			}

			// Material collision detection with projectiles
			int cellX = static_cast<int>(projectile.position.x) / CELL_SIZE;
			int cellY = static_cast<int>(projectile.position.y) / CELL_SIZE;

			// Check projectile collision with materials
			if (currentRoom().getWorld().getCell(cellX, cellY) != MaterialType::Empty)
			{
				currentRoom().getWorld().explodeParticles(cellX, cellY, static_cast<int>(projectile.explosionRadius) / CELL_SIZE);
				applyPlayerExplosionKnockback(projectile.position, projectile.explosionRadius, projectile.playerKnockback);

				projectile.isAlive = false;

				printf("projectile hit material\n");
			}
		}

		// Collectable updates and collision detection
		for (auto& collectable : currentRoom().getCollectables())
		{
			bool wasCollected = collectable.isCollected();
			
			collectable.update(playerBounds);

			collectable.update(playerBounds);
			if (!wasCollected && collectable.isCollected())
			{
				m_totalCollectables++;
				printf("Total collectables collected %d\n", m_totalCollectables);
			}
		}

		// Erase projectiles at end of lifetime
		m_projectiles.erase(std::remove_if(m_projectiles.begin(), m_projectiles.end(), [](const Projectile& p)
			{
				return !p.isAlive;
			}),
			m_projectiles.end());

		// Only update player input in gameplay mode
		m_playerController.update(m_player, currentRoom().getTileMap(), dt);

		// Update player animations to reflect player input
		m_player.updateAnimation(dt);
	}

	// If the editor mode is active, allow editing the tile map with the left and right mouse buttons
	if (m_editorState == GameState::Editor)
	{
		// ----------- Mouse Input -----------
		if (!mouseOverPalette)
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
			{
				currentRoom().getTileMap().setTile(xTile, yTile, m_selectedTileType);
			}
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
		{
			// Erase tiles and cells
			currentRoom().getTileMap().setTile(xTile, yTile, TileType::Empty);
			currentRoom().getWorld().paintCircle(xCell, yCell, m_brushSize, MaterialType::Empty);
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle))
		{
			currentRoom().getWorld().paintCircle(xCell, yCell, m_brushSize, m_selectedMaterial);
		}

		// ----------- Keyboard Input -----------
		bool wHeld = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W);
		if (wHeld && !m_keyHeldLastFrameW)
			switchRoom({ 0, -1 });
		m_keyHeldLastFrameW = wHeld;

		bool aHeld = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
		if (aHeld && !m_keyHeldLastFrameA)
			switchRoom({ -1, 0 });
		m_keyHeldLastFrameA = aHeld;

		bool sHeld = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
		if (sHeld && !m_keyHeldLastFrameS)
			switchRoom({ 0, 1 });
		m_keyHeldLastFrameS = sHeld;

		bool dHeld = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
		if (dHeld && !m_keyHeldLastFrameD)
			switchRoom({ 1, 0 });
		m_keyHeldLastFrameD = dHeld;

		// Delete room
		bool deleteHeld = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Delete);
		if (deleteHeld && !m_keyHeldLastFrameDel)
			m_room_manager.deleteCurrentRoom();
		m_keyHeldLastFrameDel = deleteHeld;
	}
}

void Game::render()
{
	m_gameTexture.clear(BACKGROUND_COLOR);

	// Draw room
	currentRoom().draw(m_gameTexture);

	// ------- GAMEPLAY -------
	if (m_editorState == GameState::Gameplay)
	{
		// Draw player
		m_player.draw(m_gameTexture);

		for (const auto& projectile : m_projectiles)
		{
			projectile.draw(m_gameTexture);
		}
	}

	// ------- EDITOR -------
	if (m_editorState == GameState::Editor)
	{
		sf::VertexArray gridLines(sf::PrimitiveType::Lines);

		// Vertical lines
		for (int x = 0; x <= GAME_WIDTH / TILE_SIZE; ++x)
		{
			gridLines.append(sf::Vertex(sf::Vector2f(static_cast<float>(x * TILE_SIZE), 0.f), sf::Color(255, 255, 255, 50)));
			gridLines.append(sf::Vertex(sf::Vector2f(static_cast<float>(x * TILE_SIZE), static_cast<float>(GAME_HEIGHT)), sf::Color(255, 255, 255, 50)));
		}

		// Horizontal lines
		for (int y = 0; y <= GAME_HEIGHT / TILE_SIZE; ++y)
		{
			gridLines.append(sf::Vertex(sf::Vector2f(0.f, static_cast<float>(y * TILE_SIZE)), sf::Color(255, 255, 255, 50)));
			gridLines.append(sf::Vertex(sf::Vector2f(static_cast<float>(GAME_WIDTH), static_cast<float>(y * TILE_SIZE)), sf::Color(255, 255, 255, 50)));
		}

		m_gameTexture.draw(gridLines);
		
		// Brush stuff
		sf::CircleShape brush;

		brush.setRadius(static_cast<float>(m_brushSize * CELL_SIZE));
		brush.setFillColor(sf::Color::Transparent);
		brush.setOutlineColor(sf::Color::White);
		brush.setOutlineThickness(2.f);

		auto mousePos = sf::Mouse::getPosition(m_window);

		float scaleX = static_cast<float>(WINDOW_WIDTH) / GAME_WIDTH;
		float scaleY = static_cast<float>(WINDOW_HEIGHT) / GAME_HEIGHT;

		sf::Vector2f gameMouse(mousePos.x / scaleX, mousePos.y / scaleY);

		brush.setPosition({ gameMouse.x - (m_brushSize * CELL_SIZE), gameMouse.y - (m_brushSize * CELL_SIZE)});

		m_gameTexture.draw(brush);

		Game::drawMaterialUI();
		Game::drawTilePalette();
	}

	// Finalise game texture
	m_gameTexture.display();

	m_window.clear();

	sf::Sprite gameSprite(m_gameTexture.getTexture());

	gameSprite.setScale({ static_cast<float>(WINDOW_WIDTH / GAME_WIDTH), static_cast<float>(WINDOW_HEIGHT / GAME_HEIGHT) });

	m_window.draw(gameSprite);

	// Calculate and display FPS
	float fps = 1.f / m_fps;
	m_fpsText.setString("FPS: " + std::to_string(static_cast<int>(fps)));

	// Draw the FPS text on the window
	m_window.draw(m_fpsText);

	// End the current frame and display its contents on screen
	m_window.display();
}

void Game::drawMaterialUI()
{
	const int SIZE = 8;
	const int PADDING = 3;

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
			rect.setOutlineThickness(1.f);
		}
		else
		{
			rect.setOutlineColor(sf::Color::Transparent);
			rect.setOutlineThickness(0.f);
		}

		m_gameTexture.draw(rect);
	}
}

void Game::drawTilePalette()
{
	// Set atlas
	sf::Sprite atlas(currentRoom().getTileMap().getTileset());

	atlas.setPosition({ 20.f, 40.f });

	m_gameTexture.draw(atlas);

	// Store clickable bounds
	m_tilePaletteBounds = atlas.getGlobalBounds();

	// Set selector
	sf::RectangleShape selector;

	selector.setSize({ static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE) });
	selector.setFillColor(sf::Color::Transparent);
	selector.setOutlineThickness(2.f);
	selector.setOutlineColor(sf::Color::Yellow);

	// Compute selected atlas index
	int selectedIndex = 0;

	switch (m_selectedTileType)
	{
	case TileType::Solid:
		selectedIndex = 0;
		break;

	case TileType::Spike:
		selectedIndex = 1;
		break;

	case TileType::Bounce:
		selectedIndex = 2;
		break;
	}

	int atlasColumns = currentRoom().getTileMap().getTileset().getSize().x / TILE_SIZE;

	int sx = selectedIndex % atlasColumns;
	int sy = selectedIndex / atlasColumns;

	selector.setPosition({ 20.f + sx * TILE_SIZE, 40.f + sy * TILE_SIZE });

	m_gameTexture.draw(selector);
}

void Game::handleTilePaletteClick(sf::Vector2i mousePos)
{
	if (!m_tilePaletteBounds.contains(static_cast<sf::Vector2f>(mousePos)))
		return;

	int localX = static_cast<int>(mousePos.x - m_tilePaletteBounds.position.x);
	int localY = static_cast<int>(mousePos.y - m_tilePaletteBounds.position.y);

	int tileX = localX / TILE_SIZE;
	int tileY = localY / TILE_SIZE;

	int atlasColumns = currentRoom().getTileMap().getTileset().getSize().x / TILE_SIZE;

	int tileIndex = tileX + tileY * atlasColumns;

	switch (tileIndex)
	{
	case 0:
		m_selectedTileType = TileType::Solid;
		break;
		
	case 1:
		m_selectedTileType = TileType	::Spike;
		break;

	case 2:
		m_selectedTileType = TileType::Bounce;
		break;
	}
}

void Game::switchRoom(sf::Vector2i direction)
{
	Room& room = currentRoom();

	sf::Vector2i currentGrid = room.getGridPosition();
	sf::Vector2i targetGrid = { currentGrid.x + direction.x, currentGrid.y + direction.y };

	int roomIndex = m_room_manager.findRoomAtGridPosition(targetGrid);

	if (roomIndex == -1)
	{
		m_room_manager.createRoom(targetGrid);

		roomIndex = m_room_manager.findRoomAtGridPosition(targetGrid);
	}

	m_room_manager.setCurrentRoom(roomIndex);

	sf::Vector2f pos = m_player.position;

	// Set player spawns when entering a new room
	if (direction.x > 0)		
		pos.x = 0.f;

	else if (direction.x < 0)
		pos.x = GAME_WIDTH;

	if (direction.y > 0)
		pos.y = 0.f;

	else if (direction.y < 0)
		pos.y = GAME_HEIGHT;

	m_player.position = pos;

	printf("current grid coordinates: %d, %d\n", targetGrid.x, targetGrid.y);
}

void Game::applyPlayerExplosionKnockback(sf::Vector2f explosionPos, float radius, float force)
{
	sf::Vector2f playerCentre = { m_player.position.x + m_player.size.x * 0.5f, m_player.position.y + m_player.size.y * 0.5f };
	
	sf::Vector2f diff = playerCentre - explosionPos;

	float distSq = (diff.x * diff.x) + (diff.y * diff.y);

	if (distSq > radius * radius)
		return;

	float dist = std::sqrt(distSq);

	if (dist < 0.001f)
		dist = 0.001f;

	sf::Vector2f dir = diff / dist;

	float strength = force * (1.f - (dist / radius));

	m_player.applyKnockback(dir * strength);

	printf("Player konckback trength: %f\n", strength);
}

void Game::fireProjectile()
{
	sf::Vector2f dir = { 0.f, 0.f };

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
		dir = { 0.f, -1.f };

	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
		dir = { 0.f, 1.f };

	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
		dir = { -1.f, 0.f };

	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
		dir = { 1.f, 0.f };

	// Default direction if no input (also check player facing direction)
	else
	{
		dir = m_player.facingRight ? sf::Vector2f(1.f, 0.f) : sf::Vector2f(-1.f, 0.f);
	}

	Projectile projectile;

	float projectileOffsetX = m_player.size.x * 0.5f;
	float projectileOffsetY = m_player.size.y * 0.5f;

	// Initial values of the projectile
	projectile.position.x = m_player.position.x + projectileOffsetX;
	projectile.position.y = m_player.position.y + projectileOffsetY;
	
	projectile.velocity = dir * projectile.speed;

	m_projectiles.push_back(projectile);
}

sf::Vector2f Game::getMouseGamePosition() const
{
	sf::Vector2i mouse = sf::Mouse::getPosition(m_window);

	float scaleX = static_cast<float>(WINDOW_WIDTH) / GAME_WIDTH;
	float scaleY = static_cast<float>(WINDOW_HEIGHT) / GAME_HEIGHT;

	return { mouse.x / scaleX, mouse.y / scaleY };
}