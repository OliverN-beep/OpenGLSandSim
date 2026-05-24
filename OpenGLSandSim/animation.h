#pragma once

#include <vector>

struct Animation
{
	std::vector<int> frames;

	float frameTime = 0.1f;
	bool looping = true;
};