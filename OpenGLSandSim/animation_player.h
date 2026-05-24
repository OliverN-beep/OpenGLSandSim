#pragma once

#include <unordered_map>
#include <string>

#include "animation.h"

class AnimationPlayer
{
public:
	void addAnimation(const std::string& name, const Animation& animation);

	void play(const std::string& name);
	void update(float dt);

	int getCurrentFrame() const;

private:
	std::unordered_map<std::string, Animation> m_animations;

	Animation* m_currentAnimation = nullptr;

	std::string m_currentName;

	int m_frameIndex = 0;

	float m_timer = 0.f;
};