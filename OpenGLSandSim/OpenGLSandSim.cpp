// OpenGLSandSim.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

int main()
{
	// Create the main window
	sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "OpenGL Sand Simulation");
	window.setVerticalSyncEnabled(true);

	// load resources, initialise the OpenGL states, ...
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

	// Start the game loop
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
			else if (const auto* resized = event->getIf<sf::Event::Resized>())
			{
				// Adjust the viewport when the window is resized
				glViewport(0, 0, resized->size.x, resized->size.y);
			}
		}

		// Clear the buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw circle using RenderWindow::draw
		window.draw(shape);

		// End the current frame (internally swaps the front and back buffers)
		window.display();
	}

	// Release resources...
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
