#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

struct SandGrain
{
	// Variables to manage the sand grain's state 
	sf::RectangleShape sandGrainInstance;

	sf::Vector2f sandGrainVelocity = { 0.0f, 1.0f };
	sf::Vector2f sandGrainAcceleration = { 0.0f, 0.5f };
	sf::Vector2f sandGrainPosition = { 640.0f, 360.0f };
	sf::Vector2f sandGrainSize = { 5.0f, 5.0f };
	sf::Color sandGrainColor = sf::Color::Yellow;

	int sandGrainID = 0;			// Unique identifier for the grain
	int sandGrainLifetime = 5;		// Lifetime in seconds
	bool isGrounded = false;		// Flag to indicate if the grain is grounded
	bool hasBeenUpdated = false;	// Flag to indicate if the grain has been updated in the current frame

	// Functions to update the grain's state
	void checkCollision(SandGrain& grain, const int RW_WIDTH, const int RW_HEIGHT);
	void updateSandGrain(SandGrain& grain, const int RW_WIDTH, const int RW_HEIGHT);
};