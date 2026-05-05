#include "materials.h"
#include <iostream>

struct DebugInit
{
	DebugInit()
	{
		std::cout << "MATERIALS INITIALISED\n\n";
	}
};
static DebugInit debugInit;


MaterialProperties g_materials[(int)MaterialType::COUNT] =
{
	// BehaviorType, Density, Flammable, Wettable, Conductive, DefaultLife, Colour, Gravity, MaxVelocity, SpreadChance

	// Empty
	{BehaviourType::None, 0.f, false, false, false, 0, sf::Color(0, 0, 0, 0), 0.f, 0.f, 0.f},

	// Sand
	{BehaviourType::Powder, 1.5f, false, true, false, 0, sf::Color(194, 178, 128), 1.0f, 1.0f, 0.1f},

	// Water
	{BehaviourType::Liquid, 1.f, false, true, true, 0, sf::Color(0, 80, 255), 1.0f, 1.0f, 0.1f},

	// Stone
	{BehaviourType::Solid, 5.f, false, false, true, 0, sf::Color(128, 128, 128), 1.0f, 1.0f, 0.1f},

	// Oil
	{BehaviourType::Liquid, 0.8f, true, true, false, 0, sf::Color(0, 0, 0), 1.0f, 1.0f, 0.1f},

	// Fire
	{BehaviourType::Powder, 1.f, false, false, false, 60, sf::Color(255, 0, 0), 1.0f, 1.0f, 0.1f},

	// Smoke
	{BehaviourType::Gas, 0.f, false, false, false, 10, sf::Color(105, 105, 105), 1.0f, 1.0f, 0.1f},

	// Snow
	{BehaviourType::Powder, 0.1f, false, true, false, 0, sf::Color(255, 250, 250), 1.0f, 1.0f, 0.1f},

	// Wood
	{BehaviourType::Solid, 3.f, true, true, true, 0, sf::Color(139, 69, 19), 1.0f, 1.0f, 0.1f},

	// Salt
	{BehaviourType::Powder, 2.1f, false, true, false, 0, sf::Color(180, 180, 255), 1.0f, 1.0f, 0.1f}
};