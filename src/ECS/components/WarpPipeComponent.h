#ifndef WARPPIPECOMPONENT_H
#define WARPPIPECOMPONENT_H
#include "Component.h"
#include "../../utils/ResourceIdentifiers.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

class WarpPipeComponent : public Component
{
public:
    WarpPipeComponent(sf::Vector2i playerLocation, sf::Vector2i cameraLocation, Direction inDirection,
                      Direction outDirection, bool cameraFreeze, sf::Color backgroundColor,
                      LevelType levelType, sf::Vector2i newLevel)
        : m_playerLocation{playerLocation},
        m_cameraLocation{cameraLocation},
        m_inDirection{inDirection},
        m_outDirection{outDirection},
        m_cameraFreeze{cameraFreeze},
        m_backgroundColor{backgroundColor},
        m_levelType{levelType},
        m_newLevel{newLevel}
    {}

    sf::Vector2i &getPlayerLocation() { return m_playerLocation; }

    sf::Vector2i &getCameraLocation() { return m_cameraLocation; }

    Direction &getInDirection() { return m_inDirection; }

    Direction &getOutDirection() { return m_outDirection; }

    bool &getCameraFreeze() { return m_cameraFreeze; }

    sf::Color &getBackgroundColor() { return m_backgroundColor; }

    LevelType &getLevelType() { return m_levelType; }

    sf::Vector2i &getNewLevel() { return m_newLevel; }

private:
    sf::Vector2i m_playerLocation;
    sf::Vector2i m_cameraLocation;

    Direction m_inDirection;
    Direction m_outDirection;

    bool m_cameraFreeze;

    sf::Color m_backgroundColor;

    LevelType m_levelType;

    sf::Vector2i m_newLevel;
};

#endif // WARPPIPECOMPONENT_H
