#pragma once

#include <SFML/Graphics.hpp>

#include "animation_player.h"

class Player
{
public:
	Player(float x, float y);

	void draw(sf::RenderTarget& target) const;
	void updateAnimation(float dt);
	void applyKnockback(sf::Vector2f force);	// Function to apply knockback to player (when rocket jumping)

	sf::Vector2f position;					// Player's position
	sf::Vector2f velocity;					// Player's velocity
	sf::Vector2f size;						// Player's size

	bool grounded = false;					// Is the player on the ground?
	bool facingRight = true;				// Is the player facing right?

	float coyoteTimer = 0.f;				// Time since the player left the ground
	float jumpBufferTimer = 0.f;			// Time since the player pressed the jump button

private:
	sf::Texture m_texture;
	sf::Sprite m_sprite;

	AnimationPlayer m_animationPlayer;		// Animation player variable inherits from AnimationPlayer class
};