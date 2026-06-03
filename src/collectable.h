#pragma once

#include <SFML/Graphics.hpp>

struct Collectable
{
public:
	Collectable(sf::Vector2f pos);

	void update(sf::FloatRect playerBounds);
	void draw(sf::RenderTarget& target) const;

	bool isCollected() const;

private:
	sf::CircleShape m_shape;
	
	bool m_isCollected = false;		// Flag to indicate if the collectable has been collected by the player
};