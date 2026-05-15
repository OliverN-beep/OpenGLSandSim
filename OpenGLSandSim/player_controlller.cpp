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
	int tx = static_cast<int>(std::floor(px / map.getTileSize()));
	int ty = static_cast<int>(std::floor(py / map.getTileSize()));

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
		if (player.velocity.y > JUMP_SPEED)
			player.velocity.y = JUMP_SPEED;

		player.coyoteTimer = 0.f;		// Reset coyote timer
		player.jumpBufferTimer = 0.f;	// Reset jump buffer timer
		player.grounded = false;		// Player is no longer grounded
	}

	// Variable jump height logic
	if (!jumpHeld && player.velocity.y < -120.f)
	{
		player.velocity.y = -120.f; // Reduce upward velocity for variable jump height
	}

	// If player is moving upwards apply normal gravity, if not apply increased gravity for faster falling
	player.velocity.y += (player.velocity.y < 0.f) ? (GRAVITY * dt) : (GRAVITY * FALL_GRAVITY_MULTIPLIER * dt);

	moveAndCollide(player, map, dt); // Handle movement and collision
}

// AABB collision checker
bool PlayerController::isColliding(TileMap& map, sf::FloatRect bounds)
{
	return
		isSolidAt(map, bounds.position.x, bounds.position.y) ||
		isSolidAt(map, bounds.position.x + bounds.size.x, bounds.position.y) ||
		isSolidAt(map, bounds.position.x, bounds.position.y + bounds.size.y) ||
		isSolidAt(map, bounds.position.x + bounds.size.x, bounds.position.y + bounds.size.y);
}

void PlayerController::moveAndCollide(Player& player, TileMap& map, float dt)
{
	player.grounded = false;

	// -------- Horizontal --------
	player.position.x += player.velocity.x * dt;

	sf::FloatRect horizontalBounds(player.position, player.size);

	if (isColliding(map, horizontalBounds))
	{
		// Undo movement
		player.position.x -= player.velocity.x * dt;

		// Damp instead of hard stop
		player.velocity.x *= 0.3f;
	}

	// -------- Vertical --------
	player.position.y += player.velocity.y * dt;

	sf::FloatRect verticalBounds(player.position, player.size);

	if (isColliding(map, verticalBounds))
	{
		// Undo movement
		player.position.y -= player.velocity.y * dt;

		if (player.velocity.y > 0.f)
		{
			player.grounded = true;
			player.coyoteTimer = COYOTE_TIME;
		}

		if (std::abs(player.velocity.y) < 50.f)
		{
			// Falling to ground
			player.velocity.y = 0.f;
		}
	}
}