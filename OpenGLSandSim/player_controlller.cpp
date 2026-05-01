#include "player_controller.h"

const int CELL_SIZE = 6;						// Size of each cell in pixels

// ----------Tunable parameters----------
const float MOVE_SPEED = 200.f;					// Horizontal movement speed
const float GROUND_ACCELERATION = 1000.f;		// Acceleration when on the ground
const float AIR_ACCELERATION = 600.f;			// Acceleration when in the air
const float FRICTION = 1200.f;					// Friction applied when grounded

const float GRAVITY = 1500.f;					// Gravity force applied to the player
const float FALL_GRAVITY_MULTIPLIER = 1.5f;		// Multiplier for gravity when falling

const float JUMP_SPEED = -500.f;				// Initial jump speed

const float COYOTE_TIME = 0.1f;					// Time allowed to jump after leaving the ground
const float JUMP_BUFFER_TIME = 0.1f;			// Time allowed to jump after pressing the jump button

const float DASH_SPEED = 600.f;					// Speed during dash
const float DASH_DURATION = 0.2f;				// Duration of the dash

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

//bool PlayerController::isSolidAt(const World& world, float x, float y)
//{
	//int cellX = static_cast<int>(x) / CELL_SIZE; // Assuming each cell is 6 pixels wide
	//int cellY = static_cast<int>(y) / CELL_SIZE; // Assuming each cell is 6 pixels high

	//MaterialType matType = world.getCell(cellX, cellY);

	//return matType != MaterialType::Empty;
//}

bool PlayerController::isSolidAt(TileMap& map, float px, float py)
{
	int tx = (int)px / map.getTileSize();
	int ty = (int)py / map.getTileSize();

	return map.isSolid(tx, ty);
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

	jumpPressedLastFrame = jumpHeld; // Update the jumpPressedLastFrame state

	// Dash input handling
	static bool dashPressedLastFrame = false;
	bool dashHeld = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift);

	bool dashPressed = dashHeld && !dashPressedLastFrame; // Dash is pressed if held and wasn't pressed last frame
	dashPressedLastFrame = dashHeld; // Update the dashPressedLastFrame state

	// --------------Logic------------------

	// Horizontal movement logic
	float acceleration = player.grounded ? GROUND_ACCELERATION : AIR_ACCELERATION;

	if (input != 0.f)
	{
		player.velocity.x = moveToward(player.velocity.x, input * MOVE_SPEED, acceleration * dt);
	}
	else if (player.grounded)
	{
		player.velocity.x = moveToward(player.velocity.x, 0.f, FRICTION * dt);
	}

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
		player.velocity.y *= 0.5f; // Reduce upward velocity for variable jump height
	}

	// Dash logic
	if (dashPressed && player.canDash)
	{
		player.isDashing = true;											// Start dashing
		player.canDash = false;												// Disable further dashes until grounded
		player.dashTimer = DASH_DURATION;									// Reset dash timer
		player.velocity.x = player.facingRight ? DASH_SPEED : -DASH_SPEED;	// Set dash velocity based on facing direction
		player.velocity.y = 0.f;											// No vertical movement during dash
	}

	if (player.isDashing)
	{
		player.dashTimer -= dt; // Decrease dash timer

		if (player.dashTimer <= 0.f)
		{
			player.isDashing = false; // End dash when timer runs out
		}
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
		player.velocity.x = 0.f; // Stop horizontal movement if colliding
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
}