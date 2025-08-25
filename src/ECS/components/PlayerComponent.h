#ifndef PLAYERCOMPONENT_H
#define PLAYERCOMPONENT_H
#include "Component.h"
#include "../../utils/ResourceIdentifiers.h"

class PlayerComponent : public Component
{
public:
    PlayerComponent()
        : m_playerState{PlayerState::SmallMario}, m_superStar{false}
    {}

    void setPlayerState(PlayerState newState) { m_playerState = newState; }

    PlayerState getPlayerState() const { return m_playerState; }

    bool isSuperStar() const { return m_superStar; }

    void setSuperStar(bool on) { m_superStar = on; }

private:
    PlayerState m_playerState;
    bool m_superStar;
};

#endif // PLAYERCOMPONENT_H
