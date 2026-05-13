#include "player_controller.h"

const int CELL_SIZE = 3;						// Size of each cell in pixels

// ----------Tunable parameters----------
const float MOVE_SPEED = 320.f;					// Horizontal movement speed
const float GROUND_ACCELERATION = 1400.f;		// Acceleration when on the ground
const float AIR_ACCELERATION = 900.f;			// Acceleration when in the air
const float FRICTION = 700.f;					// Friction applied when grounded

const float GRAVITY = 1200.f;					// Gravity force applied to the player
const float FALL_GRAVITY_MULTIPLIER = 1.15f;	// Multiplier for gravity when falling

const float JUMP_SPEED = -420.f;				// Initial jump speed

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

// Collision with tiles
bool PlayerController::isSolidAt(TileMap& map, float px, float py)
{
	int tx = static_cast<int>(px) / map.getTileSize();
	int ty = static_cast<int>(py) / map.getTileSize();

	return map.isSolid(tx, ty);
}

// Collision with spikes
bool PlayerController::isSpikeAt(TileMap& map, float px, float py)
{
	int tx = static_cast<int>(px) / map.getTileSize();
	int ty = static_cast<int>(py) / map.getTileSize();
	return map.isSpike(tx, ty);
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
	player.coyoteTimer -= dt; // Decrease coyote timer
	player.jumpBufferTimer -= dt; // Decrease jump buffer timer

	// Jump input handling with jump buffering
	static bool jumpPressedLastFrame = false;
	bool jumpHeld = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);

	if (jumpHeld && !jumpPressedLastFrame)
	{
		player.jumpBufferTimer = JUMP_BUFFER_TIME; // Reset jump buffer timer when jump is pressed
	}

	// --------------Logic------------------

	// Horizontal movement logic
	float acceleration = player.grounded ? GROUND_ACCELERATION : AIR_ACCELERATION;

	if (input != 0.f)
	{
		player.velocity.x += input * acceleration * dt;
	}
	if (player.grounded && input == 0.f)
		player.velocity.x *= 0.94f;
	
	// Clamp horizontal move speed
	if (player.velocity.x > MOVE_SPEED)
		player.velocity.x = MOVE_SPEED;

	if (player.velocity.x < -MOVE_SPEED)
		player.velocity.x = -MOVE_SPEED;

	// Jumping logic
	if (player.jumpBufferTimer > 0.f && player.coyoteTimer > 0.f)
	{
		player.velocity.y = JUMP_SPEED; // Apply jump speed
		player.grounded = false;		// Player is no longer grounded
		player.coyoteTimer = 0.f;		// Reset coyote timer
		player.jumpBufferTimer = 0.f;	// Reset jump buffer timer
	}

	// Variable jump height logic
	if (!jumpHeld && player.velocity.y < 0.f)
	{
		player.velocity.y *= 0.92f; // Reduce upward velocity for variable jump height
	}
	else
	{
		// Gravity logic
		if (player.velocity.y < 0.f) // If moving upwards
		{
			player.velocity.y += GRAVITY * dt; // Apply normal gravity
		}
		else // If falling
		{
			player.velocity.y += GRAVITY * FALL_GRAVITY_MULTIPLIER * dt; // Apply increased gravity for faster fall
		}

		moveAndCollide(player, map, dt); // Handle movement and collision
	}
}

void PlayerController::moveAndCollide(Player& player, TileMap& map, float dt)
{
	// Calculate the new position based on velocity and delta time
	sf::Vector2f newPosition = player.position + player.velocity * dt;

	// Check for horizontal collisions
	if (isSolidAt(map, newPosition.x, player.position.y) || isSolidAt(map, newPosition.x + player.size.x, player.position.y))
	{
		player.velocity.x *= 0.2f; // Slow horizontal movement if colliding
	}
	else
	{
		player.position.x = newPosition.x; // Update horizontal position if no collision
	}

	// Check for vertical collisions
	if (isSolidAt(map, player.position.x, newPosition.y) || isSolidAt(map, player.position.x + player.size.x, newPosition.y + player.size.y))
	{
		if (player.velocity.y > 0.f) // If falling
		{
			player.grounded = true; // Player is grounded
			player.coyoteTimer = COYOTE_TIME; // Reset coyote timer
		}
		player.velocity.y = 0.f; // Stop vertical movement if colliding
	}
	else
	{
		player.position.y = newPosition.y; // Update vertical position if no collision
		player.grounded = false; // Player is not grounded
	}

	if (isSpikeAt(map, player.position.x, player.position.y) || isSpikeAt(map, player.position.x + player.size.x, player.position.y + player.size.y))
	{
		// Handle spike collision (e.g., reset player position)
		player.position = sf::Vector2f(100.f, 100.f); // Reset to starting position
		player.velocity = sf::Vector2f(0.f, 0.f); // Reset velocity
		player.grounded = false; // Player is not grounded

		printf("Player hit a spike! Resetting position.\n"); // Debug output for spike collision
	}
}