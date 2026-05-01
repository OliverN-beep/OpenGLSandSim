#pragma once
#include "player.h"
#include "world.h"
#include "tilemap.h"


class PlayerController
{
public:
	void update(Player& player, TileMap& map, float dt);			// Update the player's state based on input and world state

private:
	float moveToward(float current, float target, float amount);	// Helper function to move a value toward a target by a specified amount
	bool isSolidAt(TileMap& map, float x, float y);					// Check if a position is solid in the tile map
	void moveAndCollide(Player& player, TileMap& map, float dt);	// Handle player movement and collision with the tile map
};