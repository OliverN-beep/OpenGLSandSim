#include "animation_player.h"

void AnimationPlayer::addAnimation(const std::string& name, const Animation& animation)
{
	m_animations[name] = animation;
}

void AnimationPlayer::play(const std::string& name)
{
	if (m_currentName == name)
		return;

	auto it = m_animations.find(name);

	if (it == m_animations.end())
	{
		printf("Animation '%s' not found!\n", name.c_str());
		return;
	}

	m_currentAnimation = &it->second;

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

	if (m_currentAnimation->frames.empty())
		return 0;

	return m_currentAnimation->frames[m_frameIndex];
}