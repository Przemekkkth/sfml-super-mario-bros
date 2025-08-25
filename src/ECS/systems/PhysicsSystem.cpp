#include "PhysicsSystem.h"
#include "../../AABB.h"
#include "../../Camera.h"
#include "../../ECS/components/BlockBumpComponent.h"
#include "../../ECS/components/BottomCollisionComponent.h"
#include "../../ECS/components/BumpableComponent.h"
#include "../../ECS/components/CollectibleComponent.h"
#include "../../ECS/components/CollisionExemptComponent.h"
#include "../../ECS/components/DestroyOutsideCameraComponent.h"
#include "../../ECS/components/EnemyComponent.h"
#include "../../ECS/components/FireBarComponent.h"
#include "../../ECS/components/ForegroundComponent.h"
#include "../../ECS/components/FrictionExemptComponent.h"
#include "../../ECS/components/FrozenComponent.h"
#include "../../ECS/components/GravityComponent.h"
#include "../../ECS/components/InvisibleBlockComponent.h"
#include "../../ECS/components/LeftCollisionComponent.h"
#include "../../ECS/components/MoveOutsideCameraComponent.h"
#include "../../ECS/components/MovingComponent.h"
#include "../../ECS/components/MovingPlatformComponent.h"
#include "../../ECS/components/ParticleComponent.h"
#include "../../ECS/components/PlatformLevelComponent.h"
#include "../../ECS/components/PlayerComponent.h"
#include "../../ECS/components/ProjectileComponent.h"
#include "../../ECS/components/RightCollisionComponent.h"
#include "../../ECS/components/TileComponent.h"
#include "../../ECS/components/TopCollisionComponent.h"
#include "../../ECS/components/TransformComponent.h"
#include "../Entity.h"
#include "../World.h"

PhysicsSystem::PhysicsSystem() {}

void PhysicsSystem::tick(World *world)
{
    // Update gravity for entities that have a gravity component
    world->find<GravityComponent, MovingComponent>([&](Entity *entity) {
        if ((!Camera::GetInstance().inCameraRange(entity->getComponent<TransformComponent>())
             && !entity->hasAny<MoveOutsideCameraComponent, PlayerComponent>())
            || entity->hasComponent<FrozenComponent>()) {
            return;
        }
        entity->getComponent<MovingComponent>()->getVelocity().y += 0.575f;
    });

    // Change the y position of the block being bumped
    world->find<BlockBumpComponent, TransformComponent>([&](Entity *entity) {
        auto blockBump = entity->getComponent<BlockBumpComponent>();

        entity->getComponent<TransformComponent>()->move(
            sf::Vector2f(0.0f, blockBump->getCurrentYChanged()));

        blockBump->setYChangeIndex(blockBump->yChangeIndex() + 1);

        if (blockBump->yChangeIndex() >= blockBump->getYChanges().size()) {
            entity->remove<BlockBumpComponent>();
        }
    });

    // Main Physics update loop
    world->find<MovingComponent, TransformComponent>([&](Entity *dynamicEntity) {
        if (dynamicEntity->hasComponent<FrozenComponent>()) {
            return;
        }

        if (!Camera::GetInstance().inCameraRange(dynamicEntity->getComponent<TransformComponent>())
            && !dynamicEntity->hasAny<MoveOutsideCameraComponent, PlayerComponent>()) {
            if (dynamicEntity->hasComponent<DestroyOutsideCameraComponent>()) {
                world->destroy(dynamicEntity);
                dynamicEntity = nullptr;
            }
            return;
        }

        auto *move = dynamicEntity->getComponent<MovingComponent>();
        auto *transform = dynamicEntity->getComponent<TransformComponent>();

        transform->move(sf::Vector2f(move->getVelocity().x, move->getVelocity().y));
        move->getVelocity().x += move->getAcceleration().x;
        move->getVelocity().y += move->getAcceleration().y;

        if (!dynamicEntity->hasAny<EnemyComponent, CollectibleComponent>()
            && !dynamicEntity->hasComponent<FrictionExemptComponent>()) {
            move->getVelocity().x *= GLOBALS::FRICTION;
        }

        if (move->getVelocity().x > GLOBALS::MAX_SPEED_X) {
            move->getVelocity().x = GLOBALS::MAX_SPEED_X;
        }
        if (move->getVelocity().y > GLOBALS::MAX_SPEED_Y) {
            move->getVelocity().y = GLOBALS::MAX_SPEED_Y;
        }

        if (move->getVelocity().x < -GLOBALS::MAX_SPEED_X) {
            move->getVelocity().x = -GLOBALS::MAX_SPEED_X;
        }

        // Main Physics update loop
        world->find<TileComponent, ForegroundComponent>([&](Entity *staticEntity) {
            // We don't check collinsions of particles
            if (dynamicEntity == staticEntity || staticEntity->hasComponent<ParticleComponent>()
                || dynamicEntity->hasComponent<ParticleComponent>()) {
                return;
            }

            CollisionDirection collidedDirectionVertical, collidedDirectionHorizontal;

            if (dynamicEntity->hasComponent<CollisionExemptComponent>()
                || staticEntity->hasComponent<InvisibleBlockComponent>()) {
                collidedDirectionVertical = checkCollisionY(staticEntity, dynamicEntity, false);
                collidedDirectionHorizontal = checkCollisionX(staticEntity, dynamicEntity, false);
            } else {
                collidedDirectionVertical = checkCollisionY(staticEntity, dynamicEntity, true);
                collidedDirectionHorizontal = checkCollisionX(staticEntity, dynamicEntity, true);

                if (collidedDirectionVertical != CollisionDirection::None) {
                    move->getVelocity().y = move->getAcceleration().y = 0.0f;
                }
                if (collidedDirectionHorizontal != CollisionDirection::None) {
                    move->getVelocity().x = move->getAcceleration().x = 0.0f;
                }
            }

            if (collidedDirectionVertical == CollisionDirection::Top) {
                dynamicEntity->addComponent<TopCollisionComponent>();
            } else if (collidedDirectionVertical == CollisionDirection::Bottom) {
                dynamicEntity->addComponent<BottomCollisionComponent>();
            }

            if (collidedDirectionHorizontal == CollisionDirection::Left) {
                dynamicEntity->addComponent<LeftCollisionComponent>();
            } else if (collidedDirectionHorizontal == CollisionDirection::Right) {
                dynamicEntity->addComponent<RightCollisionComponent>();
            }
        });

        if (std::abs(move->getVelocity().y) < GLOBALS::MARIO_ACCELERATION_X / 2.0f
            && move->getAcceleration().y == 0.0f) {
            move->getVelocity().y = 0.0f;
        }
        if (std::abs(move->getVelocity().x) < GLOBALS::MARIO_ACCELERATION_X / 2.0f
            && move->getAcceleration().x == 0.0f) {
            move->getVelocity().x = 0.0f;
        }
    });

    updateMovingPlatforms(world);
    updateFireBars(world);
    updatePlatformLevels(world);
}

CollisionDirection PhysicsSystem::checkCollisionY(Entity *staticEntity,
                                                  Entity *dynamicEntity,
                                                  bool adjustPosition)
{
    CollisionDirection direction = CollisionDirection::None;

    auto dynamicEntityTransform = dynamicEntity->getComponent<TransformComponent>();
    auto dynamicEntityMove = dynamicEntity->getComponent<MovingComponent>();

    auto staticEntityTransform = staticEntity->getComponent<TransformComponent>();
    const float staticEntityY = staticEntityTransform->getPosition().y;
    const float staticEntityH = staticEntityTransform->getHitbox().size.y;

    const float topDistance = std::abs(
        staticEntityTransform->getTop()
        - (dynamicEntityTransform->getBottom() + dynamicEntityMove->getVelocity().y));
    const float bottomDistance = std::abs(
        (dynamicEntityTransform->getTop() + dynamicEntityMove->getVelocity().y)
        - staticEntityTransform->getBottom());

    if (dynamicEntityMove->getVelocity().y >= 0.0f) {
        // If falling
        const float dynamicEntityX = dynamicEntityTransform->getPosition().x
                                     + dynamicEntityTransform->getHitbox().position.x
                                     + (GLOBALS::TILE_ROUNDNESS / 2);
        const float dynamicEntityY = dynamicEntityTransform->getPosition().y
                                     + dynamicEntityTransform->getHitbox().position.y
                                     + dynamicEntityMove->getVelocity().y;
        const float dynamicEntityW = dynamicEntityTransform->getHitbox().size.x
                                     - GLOBALS::TILE_ROUNDNESS;
        const float dynamicEntityH = dynamicEntityTransform->getHitbox().size.y;
        const float staticEntityX = staticEntityTransform->getPosition().x
                                    + (GLOBALS::TILE_ROUNDNESS / 2);
        const float staticEntityW = staticEntityTransform->getHitbox().size.x
                                    - GLOBALS::TILE_ROUNDNESS;
        if (AABB::isTotalCollision(dynamicEntityX,
                                   dynamicEntityY,
                                   dynamicEntityW,
                                   dynamicEntityH,
                                   staticEntityX,
                                   staticEntityY,
                                   staticEntityW,
                                   staticEntityH)) {
            if (topDistance < bottomDistance) {
                if (adjustPosition) {
                    dynamicEntityTransform->setBottom(staticEntityTransform->getTop());
                }

                staticEntity->addComponent<TopCollisionComponent>();
                direction = CollisionDirection::Bottom;
            }
        }
    } else {
        // Jumping
        const float dynamicEntityX = dynamicEntityTransform->getPosition().x
                                     + dynamicEntityTransform->getHitbox().position.x
                                     + (GLOBALS::TILE_ROUNDNESS);
        const float dynamicEntityY = dynamicEntityTransform->getPosition().y
                                     + dynamicEntityTransform->getHitbox().position.y
                                     + dynamicEntityMove->getVelocity().y;
        const float dynamicEntityW = dynamicEntityTransform->getHitbox().size.x
                                     - (2.0f * GLOBALS::TILE_ROUNDNESS);
        const float dynamicEntityH = dynamicEntityTransform->getHitbox().size.y;
        const float staticEntityX = staticEntityTransform->getPosition().x
                                    + (GLOBALS::TILE_ROUNDNESS);
        const float staticEntityW = staticEntityTransform->getHitbox().size.x
                                    - (2.0f * GLOBALS::TILE_ROUNDNESS);

        if (AABB::isTotalCollision(dynamicEntityX,
                                   dynamicEntityY,
                                   dynamicEntityW,
                                   dynamicEntityH,
                                   staticEntityX,
                                   staticEntityY,
                                   staticEntityW,
                                   staticEntityH)) {
            if (topDistance > bottomDistance) {
                if (adjustPosition) {
                    dynamicEntityTransform->setTop(staticEntityTransform->getBottom());
                }
                staticEntity->addComponent<BottomCollisionComponent>();
                direction = CollisionDirection::Top;
            }
        }
    }

    return direction;
}

CollisionDirection PhysicsSystem::checkCollisionX(Entity *staticEntity,
                                                  Entity *dynamicEntity,
                                                  bool adjustPosition)
{
    CollisionDirection direction = CollisionDirection::None;

    auto dynamicEntityTransform = dynamicEntity->getComponent<TransformComponent>();
    auto dynamicEntityMove = dynamicEntity->getComponent<MovingComponent>();

    auto staticEntityTransform = staticEntity->getComponent<TransformComponent>();

    const float leftDistance = std::abs((dynamicEntityTransform->getPosition().x
                                         + dynamicEntityTransform->getHitbox().position.x
                                         + dynamicEntityMove->getVelocity().x)
                                        - staticEntityTransform->getRight());
    const float rightDistance = std::abs(
        (dynamicEntityTransform->getPosition().x + dynamicEntityTransform->getHitbox().position.x
         + dynamicEntityTransform->getHitbox().size.x + dynamicEntityMove->getVelocity().x)
        - staticEntityTransform->getLeft());

    const float dynamicEntityX = dynamicEntityTransform->getPosition().x
                                 + dynamicEntityTransform->getHitbox().position.x
                                 + dynamicEntityMove->getVelocity().x;
    const float dynamicEntityY = dynamicEntityTransform->getPosition().y
                                 + dynamicEntityTransform->getHitbox().position.y;
    const float dynamicEntityW = dynamicEntityTransform->getHitbox().size.x;
    const float dynamicEntityH = dynamicEntityTransform->getHitbox().size.y
                                 - (GLOBALS::TILE_ROUNDNESS * 2);

    if (AABB::isTotalCollision(dynamicEntityX,
                               dynamicEntityY,
                               dynamicEntityW,
                               dynamicEntityH,
                               staticEntityTransform)) {
        if (leftDistance < rightDistance) {
            if (adjustPosition) {
                //Entity is inside block, push out
                if (dynamicEntityTransform->getLeft() < staticEntityTransform->getRight()) {
                    dynamicEntityTransform->move(
                        sf::Vector2f(std::min(0.5f,
                                              staticEntityTransform->getRight()
                                                  - dynamicEntityTransform->getLeft()),
                                     0.0f));
                } else {
                    // The entity is about to get inside the block
                    dynamicEntityTransform->setLeft(staticEntityTransform->getRight());
                }
            }
            staticEntity->addComponent<RightCollisionComponent>();
            direction = CollisionDirection::Left;
        } else {
            if (adjustPosition) {
                // Entity is inside block, push out
                if (dynamicEntityTransform->getRight() > staticEntityTransform->getLeft()) {
                    dynamicEntityTransform->move(
                        sf::Vector2f(-(std::min(0.5f,
                                                dynamicEntityTransform->getRight()
                                                    - staticEntityTransform->getLeft())),
                                     0.0f));
                } else {
                    // Entity is about inside the block
                    dynamicEntityTransform->setRight(staticEntityTransform->getLeft());
                }
            }
            staticEntity->addComponent<LeftCollisionComponent>();
            direction = CollisionDirection::Right;
        }
    }

    return direction;
}

void PhysicsSystem::updateMovingPlatforms(World *world)
{
    world->find<MovingPlatformComponent, MovingComponent, TransformComponent>([&](Entity *entity) {
        auto *platform = entity->getComponent<MovingPlatformComponent>();
        auto *platformMove = entity->getComponent<MovingComponent>();
        auto *position = entity->getComponent<TransformComponent>();

        switch (platform->getMotionType()) {
        case PlatformMotionType::OneDirectionRepeated:
            switch (platform->getDirection()) {
            case Direction::Left:
            case Direction::Right:
                if (position->getPosition().x < platform->getMinPoint()) {
                    position->setLeft(platform->getMaxPoint());
                } else if (position->getPosition().x > platform->getMaxPoint()) {
                    position->setLeft(platform->getMinPoint());
                }
                break;
            case Direction::Up:
            case Direction::Down:
                if (position->getPosition().y > platform->getMaxPoint()) {
                    position->setTop(platform->getMinPoint());
                } else if (position->getPosition().y < platform->getMinPoint()) {
                    position->setTop(platform->getMaxPoint());
                }
                break;
            default:
                break;
            }
            break;
        case PlatformMotionType::BackAndForth:
            switch (platform->getDirection()) {
            case Direction::Left:
                if (position->getLeft() <= platform->getMinPoint()) {
                    platform->getDirection() = Direction::Right;
                    break;
                }
                {
                    float newVelocity = -platform->calculateVelocity(position->getRight()
                                                                         - platform->getMinPoint(),
                                                                     (platform->getMaxPoint()
                                                                      - platform->getMinPoint())
                                                                         / 3.8f);

                    platformMove->getVelocity().x = newVelocity;
                }
                break;
            case Direction::Right:
                if (position->getRight() >= platform->getMaxPoint()) {
                    platform->getDirection() = Direction::Left;
                    break;
                }
                {
                    float newVelocity = platform->calculateVelocity(platform->getMaxPoint()
                                                                        - position->getLeft(),
                                                                    (platform->getMaxPoint()
                                                                     - platform->getMinPoint())
                                                                        / 3.8f);

                    platformMove->getVelocity().x = newVelocity;
                }
                break;
            case Direction::Up:
                if (position->getTop() <= platform->getMinPoint()) {
                    platform->getDirection() = Direction::Down;
                    break;
                }
                {
                    float newVelocity = -platform->calculateVelocity(position->getBottom()
                                                                         - platform->getMinPoint(),
                                                                     (platform->getMaxPoint()
                                                                      - platform->getMinPoint())
                                                                         / 3.8f);

                    platformMove->getVelocity().y = newVelocity;
                }
                break;
            case Direction::Down:
                if (position->getBottom() >= platform->getMaxPoint()) {
                    platform->getDirection() = Direction::Up;
                    break;
                }
                {
                    float newVelocity = platform->calculateVelocity(platform->getMaxPoint()
                                                                        - position->getTop(),
                                                                    (platform->getMaxPoint()
                                                                     - platform->getMinPoint())
                                                                        / 3.8f);

                    platformMove->getVelocity().y = newVelocity;
                }
                break;
            default:
                break;
            }
            break;
        case PlatformMotionType::Gravity: {
            if (entity->hasComponent<TopCollisionComponent>()) {
                platformMove->getAcceleration().y = 0.10f;
            } else {
                platformMove->getAcceleration().y = 0.f;
                platformMove->getVelocity().y *= 0.92f;
            }

            entity->remove<TopCollisionComponent>();
        } break;
        default:
            break;
        }
    });
}

void PhysicsSystem::updateFireBars(World *world)
{
    world->find<FireBarComponent>([](Entity *entity) {
        auto *fireBar = entity->getComponent<FireBarComponent>();
        auto *transform = entity->getComponent<TransformComponent>();

        if (fireBar->getBarAngle() > 360) {
            fireBar->getBarAngle() -= 360;
        } else if (fireBar->getBarAngle() < 0) {
            fireBar->getBarAngle() += 360;
        }

        transform->setLeft(fireBar->calculateXPosition(fireBar->getBarAngle())
                           + fireBar->getPointOfRotation().x);
        transform->setTop(fireBar->calculateYPosition(fireBar->getBarAngle())
                          + fireBar->getPointOfRotation().y);
    });
}

void PhysicsSystem::updatePlatformLevels(World *world)
{
    world->find<PlatformLevelComponent>([](Entity *entity) {
        auto *platformLevel = entity->getComponent<PlatformLevelComponent>();
        auto *platformPosition = entity->getComponent<TransformComponent>();
        auto *platformMove = entity->getComponent<MovingComponent>();

        if (!Camera::GetInstance().inCameraRange(platformPosition)) {
            return;
        }

        //return; //mine
        // auto* linePosition = platformLevel->pulleyLine->getComponent<PositionComponent>();
        // linePosition->scale.y = platformPosition->getTop() - linePosition->getTop();

        Entity *otherPlatform = platformLevel->getOtherPlatform();
        //If the level reaches max height
        if (platformPosition->getTop() < platformLevel->getPulleyHeight()) {
            platformPosition->setTop(platformLevel->getPulleyHeight());
            platformMove->getVelocity() = sf::Vector2f(0.0f, 0.0f);

            otherPlatform->getComponent<MovingComponent>()->getAcceleration().y = 0.0f;
            otherPlatform->addComponent<GravityComponent>();
            otherPlatform->addComponent<CollisionExemptComponent>();
            otherPlatform->addComponent<DestroyOutsideCameraComponent>();

            otherPlatform->remove<PlatformLevelComponent>();
            entity->remove<PlatformLevelComponent>();
            return;
        }

        if (!entity->hasComponent<TopCollisionComponent>()) {
            // Slows the platform down if the other platform isn't accelerating
            if (otherPlatform->getComponent<MovingComponent>()->getAcceleration().y == 0.0f) {
                platformMove->getVelocity().y *= 0.92;

                // Sets the 2 platforms to have opposite velocities
                otherPlatform->getComponent<MovingComponent>()->getVelocity().y
                    = -platformMove->getVelocity().y;
            }

            platformMove->getAcceleration().y = 0.0f;
            return;
        }

        platformMove->getAcceleration().y = 0.12f;

        // Sets the 2 platforms to have opposite velocities
        otherPlatform->getComponent<MovingComponent>()->getVelocity().y
            = -platformMove->getVelocity().y;

        if (platformMove->getVelocity().y > 1.0f) {
            platformMove->getVelocity().y = 1.0f;
        } else if (platformMove->getVelocity().y < -1.0f) {
            platformMove->getVelocity().y = -1.0f;
        }

        entity->remove<TopCollisionComponent>();
    });
}
