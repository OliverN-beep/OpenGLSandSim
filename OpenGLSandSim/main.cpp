#include "world.h"
#include <iostream>

int main()
{
	const int CELL_SIZE = 5;
	const int COLUMNS = 1280 / CELL_SIZE;
	const int ROWS = 720 / CELL_SIZE;

	const sf::Color BACKGROUND_COLOR = sf::Color(0, 105, 155);

	// Create the main window
	sf::RenderWindow window(sf::VideoMode({ COLUMNS * CELL_SIZE, ROWS * CELL_SIZE }), "OpenGL Sand Simulation");
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);

	World world(COLUMNS, ROWS, CELL_SIZE);

	// Set up font to display text
	sf::Font font;
	if (!font.openFromFile("fonts/monospace_medium.ttf"))
	{
		std::cerr << "Could not load font" << std::endl;
		return -1;
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
			
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
			{
				auto mousePos = sf::Mouse::getPosition(window);

				int x = mousePos.x / CELL_SIZE;
				int y = mousePos.y / CELL_SIZE;

				// Set the cell to "MaterialType::XXXX" when the left mouse button is pressed
				world.setCell(x, y, MaterialType::Sand);
			}
		}
		
		world.update();

		window.clear(BACKGROUND_COLOR);	
		world.draw(window);
		window.display();
	}

	// Loop end: release resources...
	
}
