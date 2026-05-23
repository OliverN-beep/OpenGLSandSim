#include "player.h"

Player::Player(float x, float y)
{
	position = { x, y };
	velocity = { 0.f, 0.f };
	size = { 20.f, 30.f };
}

void Player::draw(sf::RenderTarget& target) const
{
	sf::Texture playerTex("player/player_d.png");
	sf::Sprite playerSprite(playerTex);
	playerSprite.setPosition(position);

	target.draw(playerSprite);
}

void Player::applyKnockback(sf::Vector2f force)
{
	velocity += force;
}