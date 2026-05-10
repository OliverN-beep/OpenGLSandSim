#pragma once

#include <SFML/Graphics.hpp>

class Projectile
{
public:
	sf::Vector2f position;	// Current projectile position
	sf::Vector2f velocity;	// Current projectile velocity

	float radius = 4.f;		// Projectile radius

	bool is_alive = false;	// Check whether the projectile is alive

	void update(float dt);
	void draw(sf::RenderWindow& window);

private:
	sf::CircleShape projectileShape;
};