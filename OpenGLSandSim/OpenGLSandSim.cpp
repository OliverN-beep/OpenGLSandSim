#include "config.h"

int main()
{
	// Initialise render window width and height
	const int rwWidth = 1280;
	const int rwHeight = 720;

	// Create the main window
	sf::RenderWindow window(sf::VideoMode({ rwWidth, rwHeight }), "OpenGL Sand Simulation");
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);

	// Set up a sand grain to draw
	sf::RectangleShape sandGrain(sf::Vector2f(10.f, 10.f));
	sandGrain.setFillColor(sf::Color::Green);
	sandGrain.setPosition({ 640.0f, 20.0f });

	// Set up font and text to display
	sf::Font font;
	if (!font.openFromFile("fonts/monospace_medium.ttf"))
	{
		std::cerr << "Could not load font" << std::endl;
		return -1;
	}
	sf::Text text(font);
	text.setString("Hello, OpenGL with SFML!");
	text.setCharacterSize(24);
	text.setStyle(sf::Text::Bold);
	text.setFillColor(sf::Color::White);
	text.setPosition({ 640.0f, 50.0f });

	// ----------- Start the game loop -----------
	bool running = true;
	while (running)
	{
		// Handle events
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				// End program
				running = false;
			}
			if (const auto* resized = event->getIf<sf::Event::Resized>())
			{
				// Adjust the viewport when the window is resized
				glViewport(0, 0, resized->size.x, resized->size.y);
			}
		}

		// Velocity of the sand grain
		float speedX = 0.0f;
		float speedY = 6.5f;

		// Update bounds of the sand grain for collision detection every frame
		sf::FloatRect sandBounds = sandGrain.getGlobalBounds();
		
		// Get current position
		sf::Vector2f pos = sandGrain.getPosition();

		// Check bottom collision
		if (sandBounds.position.y + sandBounds.size.y + speedY >= rwHeight)
		{
			std::cout << "Bottom collision!" << std::endl;

			// Stop at floor
			sandGrain.setPosition({ pos.x, rwHeight - sandBounds.size.y });
		}
		else
		{
			// Continue falling
			sandGrain.move({ speedX, speedY });
		}

		// -------- Draw OpenGL stuff here --------
		window.clear();	
		window.draw(sandGrain);
		window.draw(text);
		window.display();
	}

	// Release resources...
	
}
