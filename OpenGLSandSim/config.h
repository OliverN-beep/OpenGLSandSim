#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

struct SandGrain
{
	sf::RectangleShape sandGrainInstance;

	sf::Vector2f sandGrainVelocity = { 0.0f, 6.0f };
	sf::Vector2f sandGrainAcceleration = { 0.0f, 1.5f };
	sf::Vector2f sandGrainPosition = { 640.0f, 360.0f };
	sf::Vector2f sandGrainSize = { 10.0f, 10.0f };

	sf::Color sandGrainColor = sf::Color::Yellow;

	float sandGrainLifetime = 5.0f; // Lifetime in seconds
};