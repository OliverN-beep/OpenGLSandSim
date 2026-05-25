#include "player.h"

Player::Player(float x, float y):
	m_sprite(m_texture)
{
	position = { x, y };
	velocity = { 0.f, 0.f };
	size = { 20.f, 20.f };

	if (!m_texture.loadFromFile("player/player_d.png"))
	{
		printf("failed to load player texture\n");
	}

	m_texture.setSmooth(false);

	// First frame
	m_sprite.setTextureRect(sf::IntRect({ 0, 0 }, { 48, 48 }));

	// Define animations
	Animation Idle;
	Idle.frames = { 2, 3, 4, 5, 6, 7};
	Idle.frameTime = 0.15f;

	Animation Run;
	Run.frames = { 14, 15, 16, 17, 18, 19, 20, 21 };
	Run.frameTime = 0.06f;

	// Add animations
	m_animationPlayer.addAnimation("Idle", Idle);
	m_animationPlayer.addAnimation("Run", Run);

	// Play default animation
	m_animationPlayer.play("Idle");
}

void Player::updateAnimation(float dt)
{
	if (std::abs(velocity.x) > 5.f)
		m_animationPlayer.play("Run");
	else
		m_animationPlayer.play("Idle");

	m_animationPlayer.update(dt);

	int frame = m_animationPlayer.getCurrentFrame();

	const int FRAME_WIDTH = 20;
	const int FRAME_HEIGHT = 20;

	m_sprite.setTextureRect(sf::IntRect({ frame * FRAME_WIDTH, 0 }, { FRAME_WIDTH, FRAME_HEIGHT }));
	m_sprite.setPosition(position);

	// Flip sprite
	if (facingRight)
	{
		m_sprite.setScale({ 1.f, 1.f });
		m_sprite.setOrigin({ 0.f, 0.f });
	}
	else
	{
		m_sprite.setScale({ -1.f, 1.f });
		m_sprite.setOrigin({ static_cast<float>(FRAME_WIDTH), 0.f });
	}
}

void Player::applyKnockback(sf::Vector2f force)
{
	velocity += force;
}

void Player::draw(sf::RenderTarget& target) const
{
	target.draw(m_sprite);
}