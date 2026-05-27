#pragma once

#include <cmath>

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
	bool isSpikeAt(TileMap& map, float x, float y);					// Check if a position is a spike in the tile map
	bool isBounceAt(TileMap& map, float x, float y);				// Check if a position is a bounce pad in the tile map

	bool isCollidingSolid(TileMap& map, sf::FloatRect bounds);		// AABB collision check helper for solid tiles
	bool isCollidingSpike(TileMap& map, sf::FloatRect bounds);		// AABB collision check helper for spike tiles
	bool isCollidingBounce(TileMap& map, sf::FloatRect bounds);		// AABB collision check helper for bounce tiles

	void moveAndCollide(Player& player, TileMap& map, float dt);	// Handle player movement and collision with the tile map
};