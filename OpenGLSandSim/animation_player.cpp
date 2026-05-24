#include "animation_player.h"

void AnimationPlayer::addAnimation(const std::string& name, const Animation& animation)
{
	m_animations[name] = animation;
}

void AnimationPlayer::play(const std::string& name)
{
	if (m_currentName == name)
		return;

	m_currentAnimation = &m_animations[name];

	m_currentName = name;

	m_frameIndex = 0;
	m_timer = 0.f;
}

void AnimationPlayer::update(float dt)
{
	if (!m_currentAnimation)
		return;

	m_timer += dt;

	while (m_timer >= m_currentAnimation->frameTime)
	{
		m_timer -= m_currentAnimation->frameTime;

		m_frameIndex++;

		if (m_frameIndex >= m_currentAnimation->frames.size())
		{
			if (m_currentAnimation->looping)
			{
				m_frameIndex = 0;
			}
			else
			{
				m_frameIndex = static_cast<int>(m_currentAnimation->frames.size()) - 1;
			}
		}
	}
}

int AnimationPlayer::getCurrentFrame() const
{
	if (!m_currentAnimation)
		return 0;

	return m_currentAnimation->frames[m_frameIndex];
}