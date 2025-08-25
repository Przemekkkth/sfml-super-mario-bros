#ifndef MOVINGCOMPONENT_H
#define MOVINGCOMPONENT_H
#include "Component.h"
#include <SFML/System/Vector2.hpp>

class MovingComponent : public Component
{
public:
    MovingComponent(sf::Vector2f velocity, sf::Vector2f acceleration)
        : m_velocity{velocity}, m_acceleration{acceleration}
    {}

    sf::Vector2f &getVelocity() { return m_velocity; }
    sf::Vector2f &getAcceleration() { return m_acceleration; }

private:

    sf::Vector2f m_velocity;
    sf::Vector2f m_acceleration;
};

#endif // MOVINGCOMPONENT_H
