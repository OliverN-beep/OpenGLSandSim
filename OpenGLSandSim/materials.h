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

enum class BehaviorType		// Enum representing different behavior types for materials
{
	None,					// Represents no specific behavior
	Solid,					// Represents solid behavior (e.g., stone, wood)
	Powder,					// Represents powder behavior (e.g., sand, salt, snow, gunpowder)
	Liquid,					// Represents liquid behavior (e.g., water, oil)
	Gas						// Represents gas behavior (e.g., smoke)
};

struct MaterialProperties	// Struct representing properties of a material
{
	BehaviorType behavior;	// The behavior type of the material

	int density;			// Density of the material (used for falling behavior)

	bool flammable;			// Indicates if the material is flammable
	bool wettable;			// Indicates if the material can be wetted by liquids
	bool conductive;		// Indicates if the material is conductive

	uint8_t defaultLife;	// Default lifetime of the material (used for fire, smoke, etc.)


	// Material Colours
	sf::Color emptyColour = sf::Color(0, 0, 0, 0);
	sf::Color sandColour = sf::Color(194, 178, 128);
	sf::Color stoneColour = sf::Color(128, 128, 128);
	sf::Color waterColour = sf::Color(0, 80, 255);
	sf::Color oilColour = sf::Color(0, 0, 0);
	sf::Color fireColour = sf::Color(255, 0, 0);
	sf::Color smokeColour = sf::Color(105, 105, 105);
	sf::Color snowColour = sf::Color(255, 250, 250);
	sf::Color woodColour = sf::Color(139, 69, 19);
	sf::Color saltColour = sf::Color(255, 255, 255);
};