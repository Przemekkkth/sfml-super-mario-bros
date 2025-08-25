#ifndef MOVINGPLATFORMCOMPONENT_H
#define MOVINGPLATFORMCOMPONENT_H
#include "Component.h"
#include "../../utils/ResourceIdentifiers.h"

class MovingPlatformComponent : public Component
{
public:
    MovingPlatformComponent(PlatformMotionType motionType, Direction movingDirection, sf::Vector2f v)
        : m_motionType{motionType}, m_direction{movingDirection}, m_minPoint{v.x}, m_maxPoint{v.y}
    {}

    float calculateVelocity(float position, float distanceTravel)
    {
        return 2.0f * std::exp(-((std::pow(position - (1.9f * distanceTravel), 2)) / (2.0f * std::pow(distanceTravel, 2))));
    }

    PlatformMotionType &getMotionType() { return m_motionType; }

    Direction &getDirection() { return m_direction; }

    float &getMinPoint() { return m_minPoint; }

    float &getMaxPoint() { return m_maxPoint; }

private:
    PlatformMotionType m_motionType;
    Direction m_direction;
    float m_minPoint;
    float m_maxPoint;
};

#endif // MOVINGPLATFORMCOMPONENT_H

