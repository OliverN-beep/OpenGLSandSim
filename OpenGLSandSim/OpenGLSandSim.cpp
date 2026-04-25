#include "config.h"

int main()
{
	const int rwWidth = 1280;
	const int rwHeight = 720;

	// Create the main window
	sf::RenderWindow window(sf::VideoMode({ rwWidth, rwHeight }), "OpenGL Sand Simulation");
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);

	// -------- Load resources, initialize the OpenGL states, ... --------

	// Set up a rectangle shape to draw
	sf::RectangleShape sandGrain(sf::Vector2f(10.f, 10.f));
	sandGrain.setFillColor(sf::Color::Green);
	sandGrain.setPosition({ 640.0f, 20.0f });

	sf::FloatRect bounds = sandGrain.getGlobalBounds();

	// Visual for bounding box
	sf::RectangleShape box(sf::Vector2f(20.f, 20.f));
	box.setFillColor(sf::Color::Transparent);
	box.setOutlineColor(sf::Color::Red);
	box.setOutlineThickness(2.0f);
	box.setPosition({ 640.0f, 20.0f });

	// Set up font
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

		sf::Vector2f point = box.getPosition();
		if (bounds.contains(point))
		{
			std::cout << "Collision :)" << std::endl;
		}
		else
		{
			std::cout << "No collision :(" << std::endl;
		}

		// Basic animation: move the circle to the right
		float sx = 0.0f;
		float sy = 2.5f;
		sandGrain.setPosition(sandGrain.getPosition() + sf::Vector2f(sx, sy));

		// -------- Draw OpenGL stuff here --------
		window.clear();	
		window.draw(sandGrain);
		window.draw(box);
		window.draw(text);
		window.display();
	}

	// Release resources...
	
}
