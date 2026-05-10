#include "projectile.h"

void Projectile::update(float dt)
{
	position += velocity * dt;

	lifetime -= dt;

	if (lifetime <= 0.f)
	{
		isAlive = false;
	}
}

void Projectile::draw(sf::RenderWindow& window) const
{
	sf::CircleShape projectileShape(radius);

	projectileShape.setFillColor(sf::Color::Red);
	projectileShape.setOrigin({ radius, radius });
	projectileShape.setPosition(position);

	window.draw(projectileShape);
}