#include "player.h"

Player::Player(float x, float y)
{
	position = { x, y };
	velocity = { 0.f, 0.f };
	size = { 20.f, 30.f };
}

void Player::draw(sf::RenderWindow& window) const
{
	sf::Texture playerTex("player/player_d.png");
	sf::Sprite playerSprite(playerTex);
	playerSprite.setPosition(position);

	sf::RectangleShape playerShape(size);
	playerShape.setPosition(position);
	playerShape.setFillColor(sf::Color::Green);

	//window.draw(playerShape);
	window.draw(playerSprite);
}

void Player::applyKnockback(sf::Vector2f force)
{
	velocity += force;
}