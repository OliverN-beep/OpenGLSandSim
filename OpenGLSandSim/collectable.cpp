#include "collectable.h"

Collectable::Collectable(sf::Vector2f pos)
{
	m_shape.setRadius(5.f);
	m_shape.setFillColor(sf::Color::Yellow);
	m_shape.setPosition(pos);
}

void Collectable::update(sf::FloatRect playerBounds)
{
	if (m_isCollected)
		return;

	if (m_shape.getGlobalBounds().findIntersection(playerBounds))
	{
		m_isCollected = true;
		printf("Collectable picked up\n");
	}
}

void Collectable::draw(sf::RenderTarget& target) const
{
	if (!m_isCollected)
		target.draw(m_shape);
}

bool Collectable::isCollected() const
{
	return m_isCollected;
}