#pragma once

#include <SFML/Graphics.hpp>

struct Projectile
{
	sf::Vector2f position;	// Current projectile position
	sf::Vector2f velocity;	// Current projectile velocity

	float radius = 6.f;				// Projectile radius
	float lifetime = 1.f;			// Projectile lifetime
	float speed = 280.f;			// Projectile speed

	float explosionRadius = 80.f;	// Explosion radius of the projectile
	float playerKnockback = 480.f;	// Knockback applied to the player
	float particleKnockback = 50.f;	// Knockback applied to particles

	bool isAlive = true;	// Check whether the projectile is alive

	void update(float dt);
	void draw(sf::RenderTarget& target) const;
};