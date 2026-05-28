#pragma once

#include <SFML/Graphics.hpp>

struct Collectable
{
public:
	void pickUpCollectable();

private:
	sf::CircleShape collectableShape;
	sf::Text countText;

	int collectableCount = 0;

};