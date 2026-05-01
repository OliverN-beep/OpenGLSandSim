#pragma once

#include <SFML/Graphics.hpp>

struct Player
{
	Player(float x, float y);

	void draw(sf::RenderWindow& window) const;

	sf::Vector2f position;					// Player's position
	sf::Vector2f velocity;					// Player's velocity
	sf::Vector2f size = {20.f, 30.f};		// Player's size (width, height)

	bool grounded = false;					// Is the player on the ground?
	bool facingRight = true;				// Is the player facing right?
	bool isJumping = false;					// Is the player currently jumping?

	float coyoteTimer = 0.f;				// Time since the player left the ground
	float jumpBufferTimer = 0.f;			// Time since the player pressed the jump button
	bool canDash = true;					// Can the player dash?
	bool isDashing = false;					// Is the player currently dashing?
	float dashTimer = 0.f;					// Time since the player started dashing
};