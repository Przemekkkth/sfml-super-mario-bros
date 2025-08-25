#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H
#include "../../utils/ResourceIdentifiers.h"
#include "System.h"
class Entity;
class TransformComponent;
class MovingComponent;
class PhysicsSystem : public System
{
public:
    PhysicsSystem();
    virtual void tick(World *world) override;

private:
    CollisionDirection checkCollisionY(Entity *staticEntity,
                                       Entity *dynamicEntity,
                                       bool adjustPosition = true);
    CollisionDirection checkCollisionX(Entity *staticEntity,
                                       Entity *dynamicEntity,
                                       bool adjustPosition = true);
    void updateMovingPlatforms(World *world);
    void updateFireBars(World *world);
    void updatePlatformLevels(World *world);
};

#endif // PHYSICSSYSTEM_H
