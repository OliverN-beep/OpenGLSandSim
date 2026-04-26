#include "config.h"

class SandGrain
{
public:
	sf::RectangleShape sandGrainInstance;
	float sandGrainSpeedX = 0.0f;
	float sandGrainSpeedY = 6.0f;
};

int main()
{
	// Initialise render window width and height
	const int RW_WIDTH = 1280;
	const int RW_HEIGHT = 720;

	// Create the main window
	sf::RenderWindow window(sf::VideoMode({ RW_WIDTH, RW_HEIGHT }), "OpenGL Sand Simulation");
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);

	// Set up a sand grain to draw
	SandGrain grain;
	grain.sandGrainInstance.setSize({ 10.0f, 10.0f });
	grain.sandGrainInstance.setFillColor(sf::Color::Yellow);
	grain.sandGrainInstance.setPosition({ 640.0f, 360.0f });

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

		// Update bounds of the sand grain for collision detection every frame
		sf::FloatRect sandBounds = grain.sandGrainInstance.getGlobalBounds();
		
		sf::Vector2f pos = grain.sandGrainInstance.getPosition();

		// Check bottom collision
		if (sandBounds.position.y + sandBounds.size.y + grain.sandGrainSpeedY >= RW_HEIGHT)
		{
			std::cout << "Bottom collision!" << std::endl;
			grain.sandGrainInstance.setPosition({ pos.x, RW_HEIGHT - sandBounds.size.y });
		}
		// Check left collision
		else if (sandBounds.position.x + grain.sandGrainSpeedX <= 0)
		{
			std::cout << "Left collision!" << std::endl;
			grain.sandGrainInstance.setPosition({ 0, pos.y });
		}
		// Check right collision
		else if (sandBounds.position.x + sandBounds.size.x + grain.sandGrainSpeedX >= RW_WIDTH)
		{
			std::cout << "Right collision!" << std::endl;
			grain.sandGrainInstance.setPosition({ RW_WIDTH - sandBounds.size.x, pos.y });
		}
		// Check top collision
		else if (sandBounds.position.y + grain.sandGrainSpeedY <= 0)
		{
			std::cout << "Top collision!" << std::endl;
			grain.sandGrainInstance.setPosition({ pos.x, 0 });
		}
		// Continue falling if no collision detected
		else
		{
			grain.sandGrainInstance.move({ grain.sandGrainSpeedX, grain.sandGrainSpeedY });
		}

		// -------- Draw OpenGL stuff here --------
		window.clear();	
		window.draw(grain.sandGrainInstance);
		window.draw(text);
		window.display();
	}

	// Loop end: release resources...
	
}
