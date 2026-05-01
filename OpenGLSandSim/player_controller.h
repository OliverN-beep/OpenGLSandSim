#pragma once
#include "player.h"
#include "world.h"
#include "tilemap.h"


class PlayerController
{
public:
    //void update(Player& player, World& world, float dt);
	void update(Player& player, TileMap& map, float dt);

private:
	float moveToward(float current, float target, float amount);
	bool isSolidAt(TileMap& map, float x, float y);
	void moveAndCollide(Player& player, TileMap& map, float dt);
};