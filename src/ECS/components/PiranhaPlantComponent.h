#ifndef PIRANHAPLANTCOMPONENT_H
#define PIRANHAPLANTCOMPONENT_H
#include "Component.h"
#include <SFML/System/Vector2.hpp>

class PiranhaPlantComponent : public Component
{
public:
    sf::Vector2f &getPipeCoordinates() { return m_pipeCoordinates; }
    bool &getInPipe() { return m_inPipe; }

private:
    sf::Vector2f m_pipeCoordinates;
    bool m_inPipe = false;
};

#endif // PIRANHAPLANTCOMPONENT_H
