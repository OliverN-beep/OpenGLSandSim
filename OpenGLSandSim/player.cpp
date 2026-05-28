#include "player.h"

Player::Player(float x, float y):
	m_sprite(m_texture)
{
	position = { x, y };
	velocity = { 0.f, 0.f };
	size = { 8.f, 14.f };		// Player collision box size (y = 15 to fit under 16x16 tiles)

	if (!m_texture.loadFromFile("player/player_d.png"))
	{
		printf("failed to load player texture\n");
	}

	m_texture.setSmooth(false);

	loadAnimations("player/player.json");

	// Play default animation
	m_animationPlayer.play("Idle");
}

void Player::loadAnimations(const std::string& jsonPath)
{
	std::ifstream file(jsonPath);

	json data;
	file >> data;

	// Parse animation tags
	for (const auto& tag : data["meta"]["frameTags"])
	{
		Animation anim;

		int from = tag["from"];
		int to = tag["to"];

		for (int i = from; i <= to; ++i)
		{
			anim.frames.push_back(i);
		}

		anim.frameTime = 0.1f;

		std::string name = tag["name"];
		
		m_animationPlayer.addAnimation(name, anim);
	}

	// Parse frame data
	for (auto& [name, frameData] : data["frames"].items())
	{
		json frame = frameData["frame"];

		m_frames.push_back(sf::IntRect({ frame["x"], frame["y"] }, { frame["w"], frame["h"] }));

		printf("Loaded animation: %s\n", name.c_str());
	}
}

void Player::updateAnimation(float dt)
{
	m_animationPlayer.update(dt);

	int frame = m_animationPlayer.getCurrentFrame();

	m_sprite.setTextureRect(m_frames[frame]);
	m_sprite.setPosition(position);

	const int FRAME_WIDTH = 20;
	const int FRAME_HEIGHT = 20;

	// Flip sprite
	if (facingRight)
	{
		m_sprite.setScale({ 1.f, 1.f });
		m_sprite.setOrigin({ static_cast<float>(FRAME_WIDTH * 0.5), 6.f });	// y offset to account for player size collision difference
	}
	else
	{
		m_sprite.setScale({ -1.f, 1.f });
		m_sprite.setOrigin({ static_cast<float>(FRAME_WIDTH), 6.f });		// y offset to account for player size collision difference
	}
}

void Player::playAnimation(const std::string& name)
{
	m_animationPlayer.play(name);
}

void Player::applyKnockback(sf::Vector2f force)
{
	velocity += force;
}

void Player::draw(sf::RenderTarget& target) const
{
	target.draw(m_sprite);
}