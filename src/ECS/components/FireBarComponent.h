#ifndef FIREBARCOMPONENT_H
#define FIREBARCOMPONENT_H
#include "Component.h"
#include "../../utils/ResourceIdentifiers.h"
#include <SFML/System/Vector2.hpp>
#include <cmath>

class FireBarComponent : public Component
{
public:
    FireBarComponent(sf::Vector2f rotationPoint, float barPosition, float startAngle,
                     RotationDirection direction)
        : m_pointOfRotation{rotationPoint}, m_barPosition{barPosition}, m_barAngle{startAngle}, m_direction{direction}
    {}

    sf::Vector2f &getPointOfRotation() { return m_pointOfRotation; }
    float &getBarPosition() { return m_barPosition; }
    float &getBarAngle() { return m_barAngle; }
    RotationDirection &getDirection() { return m_direction; }

    float calculateYPosition(float angle)
    {
        float angleRadians = angle * (M_PI / 180.0f);
        return std::sin(angleRadians) * m_barPosition;
    }

    float calculateXPosition(float angle)
    {
        float angleRadians = angle * (M_PI / 180.0f);
        return std::cos(angleRadians) * m_barPosition;
    }

private:
    sf::Vector2f m_pointOfRotation;
    float m_barPosition;
    float m_barAngle;
    RotationDirection m_direction;
};

#endif // FIREBARCOMPONENT_H
