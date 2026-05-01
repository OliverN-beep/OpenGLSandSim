#include "player.h"


Player::Player(float x, float y)
{
	position = { x, y };
	velocity = { 0.f, 0.f };
	size = { 20.f, 30.f };
}

void Player::draw(sf::RenderWindow& window) const
{
	sf::RectangleShape playerShape(size);
	playerShape.setPosition(position);
	playerShape.setFillColor(sf::Color::Green); // Set the player's color
	window.draw(playerShape);
}