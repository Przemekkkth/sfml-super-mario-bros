#include "CollectibleSystem.h"
#include "../../Globals.h"
#include "../World.h"
#include "../components/BottomCollisionComponent.h"
#include "../components/CollectibleComponent.h"
#include "../components/GravityComponent.h"
#include "../components/LeftCollisionComponent.h"
#include "../components/MovingComponent.h"
#include "../components/RightCollisionComponent.h"
#include "../components/TopCollisionComponent.h"

CollectibleSystem::CollectibleSystem() {}

void CollectibleSystem::tick(World *world)
{
    world->find<CollectibleComponent>([](Entity *entity) {
        auto *collectible = entity->getComponent<CollectibleComponent>();

        if (entity->hasComponent<LeftCollisionComponent>()
            && entity->hasComponent<GravityComponent>()) {
            entity->getComponent<MovingComponent>()->getVelocity().x = GLOBALS::COLLECTIBLE_SPEED;
        }
        if (entity->hasComponent<RightCollisionComponent>()
            && entity->hasComponent<GravityComponent>()) {
            entity->getComponent<MovingComponent>()->getVelocity().x = -GLOBALS::COLLECTIBLE_SPEED;
        }

        if (collectible->type() == CollectibleType::SuperStar) {
            if (entity->hasComponent<BottomCollisionComponent>()) {
                entity->getComponent<MovingComponent>()->getVelocity().y = -10.0f;
            }
        }

        entity->remove<TopCollisionComponent,
                       BottomCollisionComponent,
                       LeftCollisionComponent,
                       RightCollisionComponent>();
    });
}
