#ifndef GAMEPLAYSTATSSYSTEM_H
#define GAMEPLAYSTATSSYSTEM_H
#include "System.h"

class Entity;
class GameplayStatsSystem : public System
{
public:
    GameplayStatsSystem();

    virtual void onAddedToWorld(World *world) override;
    virtual void tick(World *world) override;
    virtual void handleInput(const std::optional<sf::Event> &event) override;
    virtual void onRemovedFromWorld(World *world) override;

protected:
    Entity *m_marioText;

    Entity *m_scoreEntity;

    Entity *m_coinIcon;

    Entity *m_coinsEntity;

    Entity *m_worldNumberEntity;

    Entity *m_timeEntity;

    Entity *m_timerEntity;

    Entity *m_worldNumberTransition;

    Entity *m_worldEntity;

    Entity *m_marioIcon;

    Entity *m_livesText;
};

#endif // GAMEPLAYSTATSSYSTEM_H
