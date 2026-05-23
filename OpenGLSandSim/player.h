#pragma once

#include <SFML/Graphics.hpp>

struct Player
{
	Player(float x, float y);

	void draw(sf::RenderTarget& target) const;
	void applyKnockback(sf::Vector2f force);	// Function to apply knockback to player (when rocket jumping)

	sf::Vector2f position;					// Player's position
	sf::Vector2f velocity;					// Player's velocity
	sf::Vector2f size;						// Player's size

	bool grounded = false;					// Is the player on the ground?
	bool facingRight = true;				// Is the player facing right?
	bool isJumping = false;					// Is the player currently jumping?

	float coyoteTimer = 0.f;				// Time since the player left the ground
	float jumpBufferTimer = 0.f;			// Time since the player pressed the jump button
};