#pragma once

#include <SFML/Graphics.hpp>

enum class MaterialType		// Enum representing different types of materials in the simulation
{
	Empty,					// Represents an empty cell
	Sand,					// Represents a sand cell etc...
	Stone,				
	Water,				
	Oil,					
	Fire,		
	Smoke,
	Snow,
	Wood,
	Salt,
	COUNT
};

enum class BehaviourType		// Enum representing different behavior types for materials
{
	None,						// Represents no specific behavior
	Solid,						// Represents solid behavior (e.g., stone, wood)
	Powder,						// Represents powder behavior (e.g., sand, salt, snow, gunpowder)
	Liquid,						// Represents liquid behavior (e.g., water, oil)
	Gas							// Represents gas behavior (e.g., smoke)
};

struct MaterialProperties		// Struct representing properties of a material
{
	BehaviourType behaviour;	// The behavior type of the material

	int density = 0;				// Density of the material (used for falling behavior)

	bool flammable = false;				// Indicates if the material is flammable
	bool wettable = false;				// Indicates if the material can be wetted by liquids
	bool conductive = false;			// Indicates if the material is conductive

	uint8_t defaultLife;		// Default lifetime of the material (used for fire, smoke, etc.)

	sf::Color colour = sf::Color::White;			// Color representation of the material for rendering
};

extern MaterialProperties g_materials[(int)MaterialType::COUNT];
