#ifndef HUDSYSTEM_H
#define HUDSYSTEM_H
#include "GameplayStatsSystem.h"
class HUDSystem : public GameplayStatsSystem
{
public:
    HUDSystem();
    virtual void onAddedToWorld(World *world) override;
    virtual void tick(World *world) override;
    int getGameTime() const;

    void scoreCountdown();
    bool scoreCountdownFinished() const;

private:
    void createFloatingText(World *world, Entity *originalEntity, std::string text);
    bool m_changeTime;
    int m_time;
    int m_gameTime;
    bool shouldDecreaseGameTimeByTick = true;
};

#endif // HUDSYSTEM_H
