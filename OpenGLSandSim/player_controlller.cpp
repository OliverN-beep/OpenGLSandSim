#include "player_controller.h"

// ----------Tunable parameters----------
const float MOVE_SPEED = 120.f;					// Horizontal movement speed
const float GROUND_ACCELERATION = 2000.f;		// Acceleration when on the ground
const float AIR_ACCELERATION = 800.f;			// Acceleration when in the air
const float FRICTION = 5000.f;					// Friction applied when grounded

const float GRAVITY = 900.f;					// Gravity force applied to the player
const float FALL_GRAVITY_MULTIPLIER = 0.5f;		// Multiplier for gravity when falling

const float JUMP_SPEED = -260.f;				// Initial jump speed

const float COYOTE_TIME = 0.15f;				// Time allowed to jump after leaving the ground
const float JUMP_BUFFER_TIME = 0.1f;			// Time allowed to jump after pressing the jump button

float PlayerController::moveToward(float current, float target, float amount)
{
	if (current < target)
	{
		current += amount;
		if (current > target) current = target;
	}
	else if (current > target)
	{
		current -= amount;
		if (current < target) current = target;
	}
	return current;
}

// Collision with solid tiles
bool PlayerController::isSolidAt(TileMap& map, float px, float py)
{
	int tx = static_cast<int>(std::floor(px / map.getTileSize()));
	int ty = static_cast<int>(std::floor(py / map.getTileSize()));

	return map.isSolid(tx, ty);
}

// Collision with spikes
bool PlayerController::isSpikeAt(TileMap& map, float px, float py)
{
	int tx = static_cast<int>(std::floor(px / map.getTileSize()));
	int ty = static_cast<int>(std::floor(py / map.getTileSize()));

	return map.isSpike(tx, ty);
}

// Collision with bounce pads
bool PlayerController::isBounceAt(TileMap& map, float px, float py)
{
	int tx = static_cast<int>(std::floor(px / map.getTileSize()));
	int ty = static_cast<int>(std::floor(py / map.getTileSize()));

	return map.isBounce(tx, ty);
}

void PlayerController::update(Player& player, TileMap& map, float dt)
{
	// --------------Input Handling------------------
	float input = 0.f;

	// Handle horizontal input
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
	{
		input -= 1.f;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
	{
		input += 1.f;
	}

	if (input > 0.f) player.facingRight = true;
	if (input < 0.f) player.facingRight = false;

	// Update timers
	player.coyoteTimer -= dt;		// Decrease coyote timer
	player.jumpBufferTimer -= dt;	// Decrease jump buffer timer

	// Jump input handling with jump buffering
	bool jumpHeld = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);

	static bool jumpPressedLastFrame = false;

	if (jumpHeld && !jumpPressedLastFrame)
	{
		player.jumpBufferTimer = JUMP_BUFFER_TIME; // Reset jump buffer timer when jump is pressed
	}

	jumpPressedLastFrame = jumpHeld;

	// --------------Logic------------------

	// If player is grounded, apply ground acceleration, if not apply air acceleration
	float acceleration = player.grounded ? GROUND_ACCELERATION : AIR_ACCELERATION;

	if (input != 0.f)
	{
		player.velocity.x += input * acceleration * dt;
		player.m_animationPlayer.play("Walk");
	}

	if (player.grounded && input == 0.f)
	{
		player.velocity.x = moveToward(player.velocity.x, 0.f, FRICTION * dt);
		player.m_animationPlayer.play("Idle");
	}
	
	// Clamp horizontal move speed
	if (player.velocity.x > MOVE_SPEED)
		player.velocity.x = MOVE_SPEED;

	if (player.velocity.x < -MOVE_SPEED)
		player.velocity.x = -MOVE_SPEED;

	// Jumping logic
	if (player.jumpBufferTimer > 0.f && player.coyoteTimer > 0.f)
	{
		if (player.velocity.y > JUMP_SPEED)
			player.velocity.y = JUMP_SPEED;

		player.coyoteTimer = 0.f;		// Reset coyote timer
		player.jumpBufferTimer = 0.f;	// Reset jump buffer timer
		player.grounded = false;		// Player is no longer grounded
	}

	// Only apply gravity when the player is not touching the ground
	if (!player.grounded)
	{
		// If player is moving upwards apply normal gravity, if not apply increased gravity for faster falling
		player.velocity.y += (player.velocity.y < 0.f) ? (GRAVITY * dt) : (GRAVITY * FALL_GRAVITY_MULTIPLIER * dt);
	}

	moveAndCollide(player, map, dt); // Handle movement and collision
}

// AABB collision checker
bool PlayerController::isCollidingSolid(TileMap& map, sf::FloatRect bounds)
{
	return
		isSolidAt(map, bounds.position.x, bounds.position.y) ||
		isSolidAt(map, bounds.position.x + bounds.size.x, bounds.position.y) ||
		isSolidAt(map, bounds.position.x, bounds.position.y + bounds.size.y) ||
		isSolidAt(map, bounds.position.x + bounds.size.x, bounds.position.y + bounds.size.y);
}

bool PlayerController::isCollidingSpike(TileMap& map, sf::FloatRect bounds)
{
	return
		isSpikeAt(map, bounds.position.x, bounds.position.y) ||
		isSpikeAt(map, bounds.position.x + bounds.size.x, bounds.position.y) ||
		isSpikeAt(map, bounds.position.x, bounds.position.y + bounds.size.y) ||
		isSpikeAt(map, bounds.position.x + bounds.size.x, bounds.position.y + bounds.size.y);
}

bool PlayerController::isCollidingBounce(TileMap& map, sf::FloatRect bounds)
{
	return
		isBounceAt(map, bounds.position.x, bounds.position.y) ||
		isBounceAt(map, bounds.position.x + bounds.size.x, bounds.position.y) ||
		isBounceAt(map, bounds.position.x, bounds.position.y + bounds.size.y) ||
		isBounceAt(map, bounds.position.x + bounds.size.x, bounds.position.y + bounds.size.y);
}

void PlayerController::moveAndCollide(Player& player, TileMap& map, float dt)
{
	player.grounded = false;

	// ------------ HORIZONTAL (X MOVEMENT) ------------
	// Set movement
	player.position.x += player.velocity.x * dt;

	// Define horizontal bounds of player
	sf::FloatRect horizontalBounds(player.position, player.size);

	// Set player velocity to 0 when colliding with a solid tile (x bounds)
	if (isCollidingSolid(map, horizontalBounds) || isCollidingBounce(map, horizontalBounds))
	{
		// Undo movement
		player.position.x -= player.velocity.x * dt;

		player.velocity.x = 0.f;
	}

	// ------------ VERTICAL (Y MOVEMENT) ------------
	// Set movement
	player.position.y += player.velocity.y * dt;

	// Define vertical bounds of player
	sf::FloatRect verticalBounds(player.position, player.size);

	// Set player velocity to 0 when colliding with a solid tile (y bounds)
	if (isCollidingSolid(map, verticalBounds) || isCollidingBounce(map, verticalBounds))
	{
		// Undo movement
		player.position.y -= player.velocity.y * dt;

		if (player.velocity.y > 0.f)
		{
			player.grounded = true;
			player.coyoteTimer = COYOTE_TIME;
		}

		player.velocity.y = 0.f;
	}

	// ------------ PLAYER RESPAWN ------------
	if (isCollidingSpike(map, horizontalBounds) || isCollidingSpike(map, verticalBounds))
	{
		printf("player DIED to SPIKE\n");

		// Add delay before respawning to prevent instant death from spikes

		player.position = { 70.f, 70.f };
	}
}