#include "sand_test.h"

void checkCollision(SandGrain& grain, const int RW_WIDTH, const int RW_HEIGHT)
{
	// Check bottom collision
	if (grain.sandGrainPosition.y + grain.sandGrainSize.y >= RW_HEIGHT)
	{
		std::cout << "Bottom collision!" << std::endl;

		grain.sandGrainPosition.y = RW_HEIGHT - grain.sandGrainSize.y;
		grain.sandGrainVelocity.y = 0.0f;

		grain.isGrounded = true; // Set the grain as grounded

	}
	// Check left collision
	if (grain.sandGrainPosition.x <= 0)
	{
		std::cout << "Left collision!" << std::endl;

		grain.sandGrainPosition.x = 0;
		grain.sandGrainVelocity.x = 0.0f;
	}
	// Check right collision
	if (grain.sandGrainPosition.x + grain.sandGrainSize.x >= RW_WIDTH)
	{
		std::cout << "Right collision!" << std::endl;

		grain.sandGrainPosition.x = RW_WIDTH - grain.sandGrainSize.x;
		grain.sandGrainVelocity.x = 0.0f;
	}
	// Check top collision
	if (grain.sandGrainPosition.y <= 0)
	{
		std::cout << "Top collision!" << std::endl;

		grain.sandGrainPosition.y = 0.0f;
		grain.sandGrainVelocity.y = 0.0f;
	}
}

void updateSandGrain(SandGrain& grain, const int RW_WIDTH, const int RW_HEIGHT)
{
	if (!grain.isGrounded)
	{
		grain.sandGrainVelocity += grain.sandGrainAcceleration;		// Update the grain's velocity based on acceleration (gravity)
		grain.sandGrainPosition += grain.sandGrainVelocity;			// Move the grain's logical position based on its velocity

		checkCollision(grain, RW_WIDTH, RW_HEIGHT);					// Check for collisions with the window boundaries
	}

	grain.sandGrainInstance.setPosition(grain.sandGrainPosition);	// Sync render object
}
