#ifndef FLAGSYSTEM_H
#define FLAGSYSTEM_H
#include "System.h"

class GameWorld;
class World;
class FlagSystem : public System
{
public:
    static void setClimbing(bool on);
    static bool isClimbing();
    FlagSystem(GameWorld *gameWorld);

    virtual void onAddedToWorld(World *world) override;
    virtual void tick(World *world) override;

private:
    static bool inSequence;
    static bool climbing;
    void climbFlag();
    void hitAxe();
    World *m_world;
    GameWorld *m_gameWorld;
};

#endif // FLAGSYSTEM_H
