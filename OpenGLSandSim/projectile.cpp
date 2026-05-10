#include "projectile.h"

void Projectile::update(float dt)
{
	position += velocity * dt;
}