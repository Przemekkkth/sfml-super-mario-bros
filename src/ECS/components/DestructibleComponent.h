#ifndef DESTRUCTIBLECOMPONENT_H
#define DESTRUCTIBLECOMPONENT_H
#include "Component.h"
#include <SFML/System/Vector2.hpp>

class DestructibleComponent : public Component
{
public:
    DestructibleComponent(sf::Vector2i debrisCoordinates)
        : m_debrisCoordinates{debrisCoordinates}
    {}

    sf::Vector2i getDebrisCoordinates() const { return m_debrisCoordinates; }

private:
    sf::Vector2i m_debrisCoordinates;
};

#endif // DESTRUCTIBLECOMPONENT_H
