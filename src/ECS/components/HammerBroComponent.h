#ifndef HAMMERBROCOMPONENT_H
#define HAMMERBROCOMPONENT_H
#include "Component.h"
#include "../../utils/ResourceIdentifiers.h"

class Entity;
class HammerBroComponent : public Component
{
public:
    HammerBroComponent(std::function<void(Entity *)> throwHammer)
        : m_throwHammer{throwHammer}
    {}

    Entity *getHamer() { return m_hammer; }

    std::function<void(Entity *)> &getThrowHammer() { return m_throwHammer; }

    void setHammer(Entity *entity) { m_hammer = entity; }

    int &getLastJumpTimer() { return m_lastJumpTime; }

    int &getLastThrowTimer() { return m_lastThrowTime; }

    int &getLastMoveTime() { return m_lastMoveTime; }

    Direction &getLastMoveDirection() { return m_lastMoveDirection; }

private:
    int m_lastJumpTime = 0;
    int m_lastThrowTime = 0;
    int m_lastMoveTime = 0;

    Entity* m_hammer = nullptr;

    Direction m_lastMoveDirection = Direction::None;

    std::function<void(Entity*)> m_throwHammer;
};

#endif // HAMMERBROCOMPONENT_H
