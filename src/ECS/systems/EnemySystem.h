#ifndef ENEMYSYSTEM_H
#define ENEMYSYSTEM_H
#include "System.h"

class World;
class Entity;
class EnemySystem : public System
{
public:
    EnemySystem();
    virtual void onAddedToWorld(World *world) override;
    virtual void tick(World *world) override;

private:
    void projectileBouncing();
    void checkEnemyDestroyed(Entity *enemy);
    void performBowserActions(Entity *entity);
    void performHammerBroActions(Entity *entity);
    void performLakituActions(Entity *entity);
    World *m_world;
};

#endif // ENEMYSYSTEM_H
