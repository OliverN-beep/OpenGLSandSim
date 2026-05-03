#include "materials.h"
#include <iostream>

struct DebugInit
{
	DebugInit()
	{
		std::cout << "MATERIALS INITIALISED\n";
	}
};

static DebugInit debugInit;

MaterialProperties g_materials[(int)MaterialType::COUNT] =
{
	// BehaviorType, Density, Flammable, Wettable, Conductive, DefaultLife, Colour

	// Empty
	{BehaviourType::None, 0, false, false, false, 0, sf::Color(0, 0, 0, 0)},

	// Sand
	{BehaviourType::Powder, 2, false, true, false, 0, sf::Color(194, 178, 128)},

	// Stone
	{BehaviourType::Solid, 5, false, false, true, 0, sf::Color(128, 128, 128)},

	// Water
	{BehaviourType::Liquid, 1, false, true, false, 0, sf::Color(0, 80, 255)},

	// Oil
	{BehaviourType::Liquid, 1, true, true, false, 0, sf::Color(0, 0, 0)},

	// Fire
	{BehaviourType::Powder, 0, false, false, false, 5, sf::Color(255, 0, 0)},

	// Smoke
	{BehaviourType::Gas, 0, false, false, false, 10, sf::Color(105, 105, 105)},

	// Snow
	{BehaviourType::Powder, 1, false, true, false, 0, sf::Color(255, 250, 250)},

	// Wood
	{BehaviourType::Solid, 3, true, true, true, 0, sf::Color(139, 69, 19)},

	// Salt
	{BehaviourType::Powder, 2, false, true, false, 0, sf::Color(255, 255, 255)}
};