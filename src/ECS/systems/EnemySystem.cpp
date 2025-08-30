#include "EnemySystem.h"
#include "../../AABB.h"
#include "../../Camera.h"
#include "../../ECS/components/AnimationComponent.h"
#include "../../ECS/components/BlockBumpComponent.h"
#include "../../ECS/components/BottomCollisionComponent.h"
#include "../../ECS/components/BowserComponent.h"
#include "../../ECS/components/CreateFloatingTextComponent.h"
#include "../../ECS/components/CrushableComponent.h"
#include "../../ECS/components/CrushedComponent.h"
#include "../../ECS/components/DeadComponent.h"
#include "../../ECS/components/DestroyDelayedComponent.h"
#include "../../ECS/components/DestroyOutsideCameraComponent.h"
#include "../../ECS/components/EnemyComponent.h"
#include "../../ECS/components/EnemyDestroyedComponent.h"
#include "../../ECS/components/FlagComponent.h"
#include "../../ECS/components/FrozenComponent.h"
#include "../../ECS/components/GravityComponent.h"
#include "../../ECS/components/HammerBroComponent.h"
#include "../../ECS/components/LakituComponent.h"
#include "../../ECS/components/LeftCollisionComponent.h"
#include "../../ECS/components/MoveOutsideCameraComponent.h"
#include "../../ECS/components/MovingComponent.h"
#include "../../ECS/components/ParticleComponent.h"
#include "../../ECS/components/PlayerComponent.h"
#include "../../ECS/components/ProjectileComponent.h"
#include "../../ECS/components/RightCollisionComponent.h"
#include "../../ECS/components/SoundComponent.h"
#include "../../ECS/components/TextureComponent.h"
#include "../../ECS/components/TimerComponent.h"
#include "../../ECS/components/TopCollisionComponent.h"
#include "../../ECS/components/TransformComponent.h"
#include "../../commands/CommandScheduler.h"
#include "../../commands/DelayedCommand.h"
#include "../Entity.h"
#include "../World.h"

EnemySystem::EnemySystem() {}

void EnemySystem::onAddedToWorld(World *world)
{
    m_world = world;
}

void EnemySystem::tick(World *world)
{
    projectileBouncing();
    world->find<EnemyComponent, TransformComponent>([&](Entity *enemy) {
        auto *transform = enemy->getComponent<TransformComponent>();
        auto *move = enemy->getComponent<MovingComponent>();
        auto *enemyComonent = enemy->getComponent<EnemyComponent>();

        EnemyType enemyType = enemyComonent->type();

        // Perform actions unique to each type of enemy
        switch (enemyType) {
        case EnemyType::Bowser:
            performBowserActions(enemy);
            break;
        case EnemyType::HammerBro:
            performHammerBroActions(enemy);
            break;
        case EnemyType::Lakitu:
            performLakituActions(enemy);
            break;
        case EnemyType::Spine: {
            // Turn spine eggs into spiny shells when they hit the ground
            auto *animation = enemy->getComponent<AnimationComponent>();

            if (enemy->hasComponent<BottomCollisionComponent>()
                && animation->getFrameIDs() != std::vector<int>{502, 503}) {
                std::vector<int> frameID{502, 503};
                animation->setFrameIDs(frameID);
                animation->setCurrentFrame(0);
                animation->setFrameTimer(0);
            }
        } break;
        case EnemyType::LavaBubble: {
            auto *texture = enemy->getComponent<TextureComponent>();
            // If going up and upside down
            if (move->getVelocity().y <= 0.0f && texture->isVerticalFlipped()) {
                texture->setVerticalFlipped(false);
            } else if (move->getVelocity().y > 0.0f && !texture->isVerticalFlipped()) {
                // If going down and not upside down
                texture->setVerticalFlipped(true);
            }
        } break;
        }

        // If the enemy is standing on a block and the block gets hit
        if (enemy->hasComponent<BottomCollisionComponent>()) {
            m_world->find<BlockBumpComponent>([&enemy](Entity *block) {
                int x1 = enemy->getComponent<TransformComponent>()->getPosition().x;
                int y1 = enemy->getComponent<TransformComponent>()->getPosition().y;
                int w1 = enemy->getComponent<TransformComponent>()->getHitbox().size.x;
                int h1 = enemy->getComponent<TransformComponent>()->getHitbox().size.y;
                int x2 = block->getComponent<TransformComponent>()->getPosition().x;
                int y2 = block->getComponent<TransformComponent>()->getPosition().y;
                int w2 = block->getComponent<TransformComponent>()->getHitbox().size.x;
                int h2 = block->getComponent<TransformComponent>()->getHitbox().size.y;
                if (AABB::isCollision(x1, y1, w1, h1, x2, y2, w2, h2)) {
                    enemy->addComponent<EnemyDestroyedComponent>();
                }
            });
        }

        // Enemy + Projectile collisions
        m_world->find<ProjectileComponent, MovingComponent>([&](Entity *projectile) {
            auto *projectileTransform = projectile->getComponent<TransformComponent>();
            if (!AABB::isCollision(transform, projectileTransform)
                || enemy->hasAny<ProjectileComponent, ParticleComponent>()
                || enemyType == EnemyType::LavaBubble || enemyType == EnemyType::FireBar
                || enemyType == EnemyType::BulletBill) {
                return;
            }
            if (enemy->hasComponent<BowserComponent>()) {
                if (projectile->getComponent<ProjectileComponent>()->type()
                    == ProjectileType::Fireball) {
                    // Decrease hp
                }
                return;
            }
            enemy->addComponent<EnemyDestroyedComponent>();
            m_world->destroy(projectile);
        });

        // Enemy + Enemy Collision (prevents to enemies from walking through each other)
        world->find<EnemyComponent, MovingComponent>([&](Entity *other) {
            //To Be Continued
            auto otherTransform = other->getComponent<TransformComponent>();
            if (!AABB::isCollision(transform, otherTransform) || enemy == other
                || other->hasComponent<ParticleComponent>() || enemyType == EnemyType::Spine
                || enemyType == EnemyType::BulletBill) {
                return;
            }

            if (other->getComponent<EnemyComponent>()->type() == EnemyType::KoopaShell
                && other->getComponent<MovingComponent>()->getVelocity().x != 0.0f) {
                enemy->addComponent<EnemyDestroyedComponent>();
                enemy->addComponent<MoveOutsideCameraComponent>();

                //Entity* addScore(world->create());
                //addScore->addComponent<AddScoreComponent>(100);
                return;
            }

            // If the other enemy is to the left
            if (otherTransform->getLeft() < transform->getLeft()
                && otherTransform->getRight() < transform->getRight()) {
                other->addComponent<RightCollisionComponent>();
            }
            // If the other enemy is to the right
            if (otherTransform->getLeft() > transform->getLeft()
                && otherTransform->getRight() > transform->getRight()) {
                other->addComponent<LeftCollisionComponent>();
            }
        });

        // Moves Koopas in the opposite direction if not on the ground
        if (Camera::GetInstance().inCameraRange(transform) && enemyType == EnemyType::Koopa) {
            if (!enemy->hasAny<BottomCollisionComponent, DeadComponent>()
                && enemy->getComponent<MovingComponent>()->getVelocity().y <= std::fabs(0.8f)) {
                move->getVelocity().x *= -1.0f;
                bool horizontalFlipped = enemy->getComponent<TextureComponent>()
                                             ->isHorizontalFlipped();
                enemy->getComponent<TextureComponent>()->setHorizontalFlipped(!horizontalFlipped);
            }
        }

        if (enemyType != EnemyType::PiranhaPlant && enemyType != EnemyType::CheepCheep
            && enemyType != EnemyType::Blooper && enemyType != EnemyType::Lakitu
            && enemyType != EnemyType::BulletBill) {
            // Reverses the direction of the enemy when it hits a wall or another enemy
            if (enemy->hasComponent<LeftCollisionComponent>()) {
                move->getVelocity().x = (enemyType == EnemyType::KoopaShell) ? 6.0f
                                                                             : GLOBALS::ENEMY_SPEED;
                enemy->getComponent<TextureComponent>()->setHorizontalFlipped(true);

                enemy->remove<LeftCollisionComponent>();
            } else if (enemy->hasComponent<RightCollisionComponent>()) {
                move->getVelocity().x = (enemyType == EnemyType::KoopaShell)
                                            ? -6.0f
                                            : -GLOBALS::ENEMY_SPEED;
                enemy->getComponent<TextureComponent>()->setHorizontalFlipped(false);

                enemy->remove<RightCollisionComponent>();
            }
        }

        checkEnemyDestroyed(enemy);

        if (enemyType != EnemyType::KoopaParatroopa) {
            enemy->remove<BottomCollisionComponent>();
        }

        enemy->remove<TopCollisionComponent, LeftCollisionComponent, RightCollisionComponent>();
    });
}

void EnemySystem::projectileBouncing()
{
    m_world->find<TransformComponent, MovingComponent, ProjectileComponent>([&](Entity *entity) {
        if (entity->getComponent<ProjectileComponent>()->type() == ProjectileType::Fireball) {
            if (entity->hasComponent<BottomCollisionComponent>()) {
                entity->getComponent<MovingComponent>()->getVelocity().y
                    = -GLOBALS::PROJECTILE_BOUNCE;
                entity->remove<BottomCollisionComponent>();
            }
        }
    });
}

void EnemySystem::checkEnemyDestroyed(Entity *enemy)
{
    if (enemy->hasComponent<DeadComponent>()) {
        return;
    }

    auto *move = enemy->getComponent<MovingComponent>();
    auto *enemyComponent = enemy->getComponent<EnemyComponent>();

    if (enemyComponent->type() == EnemyType::PiranhaPlant) {
        if (enemy->hasComponent<EnemyDestroyedComponent>()) {
            enemy->addComponent<ParticleComponent>();
            enemy->addComponent<DeadComponent>();

            enemy->remove<EnemyDestroyedComponent>();
            enemy->remove<AnimationComponent>();

            Entity *floatingText(m_world->create());
            floatingText->addComponent<CreateFloatingTextComponent>(enemy, std::to_string(100));

            enemy->addComponent<DestroyDelayedComponent>(1);

            Entity *kickSound = m_world->create();
            kickSound->addComponent<SoundComponent>(SoundID::Kick);
        }
        return;
    }

    // If enemy is crushed
    if (enemy->hasComponent<CrushableComponent, CrushedComponent>()) {
        // When the paratroopa gets crushed it's still crushable
        bool removeCrushable = enemyComponent->type() != EnemyType::KoopaParatroopa;
        enemy->getComponent<CrushableComponent>()->getWhenCrushed()(enemy);
        enemy->remove<CrushedComponent>();

        if (removeCrushable) {
            enemy->remove<CrushableComponent>();
        }

        Entity *floatingText(m_world->create());
        floatingText->addComponent<CreateFloatingTextComponent>(enemy, std::to_string(100));

        Entity *stompSound(m_world->create());
        stompSound->addComponent<SoundComponent>(SoundID::Stomp);
    }

    //Enemies that were destroyed through either a projectile or super star mario
    if (enemy->hasComponent<EnemyDestroyedComponent>()) {
        if (enemyComponent->type() != EnemyType::BulletBill) {
            move->getVelocity().y = -GLOBALS::ENEMY_BOUNCE;
            enemy->getComponent<TextureComponent>()->setVerticalFlipped(true);
        }

        enemy->addComponent<ParticleComponent>();
        enemy->addComponent<DeadComponent>();
        enemy->addComponent<DestroyOutsideCameraComponent>();

        enemy->remove<EnemyDestroyedComponent>();
        enemy->remove<AnimationComponent>();

        Entity *floatingText(m_world->create());
        floatingText->addComponent<CreateFloatingTextComponent>(enemy, std::to_string(100));

        Entity *destroyedSound(m_world->create());
        destroyedSound->addComponent<SoundComponent>(SoundID::Kick);
    }
}

void EnemySystem::performBowserActions(Entity *entity)
{
    if (!Camera::GetInstance().inCameraRange(entity->getComponent<TransformComponent>())
            && entity->hasComponent<FrozenComponent>()
        || entity->hasComponent<DeadComponent>()) {
        return;
    }

    auto bowserComponent = entity->getComponent<BowserComponent>();
    auto bowserTexture = entity->getComponent<TextureComponent>();
    auto player = m_world->findFirst<PlayerComponent>();

    auto flipHorizontal = player->getComponent<TransformComponent>()->getLeft()
                          > entity->getComponent<TransformComponent>()->getLeft();

    if (flipHorizontal != bowserTexture->isHorizontalFlipped()) {
        if (bowserComponent->getLastMoveDirection() == Direction::Left) {
            bowserComponent->getLastMoveDirection() = Direction::Right;
        } else {
            bowserComponent->getLastMoveDirection() = Direction::Left;
        }
        bowserTexture->setHorizontalFlipped(flipHorizontal);
    }

    bowserComponent->getLastMoveTime() = bowserComponent->getLastMoveTime() + 1;
    bowserComponent->getlastStopTime() = bowserComponent->getlastStopTime() + 1;

    bowserComponent->getLastJumpTime() = bowserComponent->getLastJumpTime() + 1;
    bowserComponent->getLastAttackTime() = bowserComponent->getLastAttackTime() + 1;

    if (bowserComponent->getCurrentMoveIndex() == 0) {
        if (bowserComponent->getlastStopTime() >= GLOBALS::FPS * 2) {
            bowserComponent->m_bowserMovements[0](entity);
            bowserComponent->m_bowserMovements[2](entity);

            bowserComponent->getCurrentMoveIndex() = bowserComponent->getCurrentMoveIndex() + 1;
        }
    } else if (bowserComponent->getCurrentMoveIndex() == 1) {
        if (bowserComponent->getlastStopTime() >= GLOBALS::FPS * 3) {
            bowserComponent->m_bowserMovements[1](entity);

            bowserComponent->getCurrentMoveIndex() = bowserComponent->getCurrentMoveIndex() + 1;
        }
    } else if (bowserComponent->getCurrentMoveIndex() == 2) {
        if (bowserComponent->getlastStopTime() >= GLOBALS::FPS * 2) {
            bowserComponent->m_bowserMovements[0](entity);
            bowserComponent->m_bowserMovements[2](entity);

            bowserComponent->getCurrentMoveIndex() = bowserComponent->getCurrentMoveIndex() + 1;
        }
    } else if (bowserComponent->getCurrentMoveIndex() == 3) {
        if (bowserComponent->getlastStopTime() >= GLOBALS::FPS * 3) {
            bowserComponent->m_bowserMovements[2](entity);

            bowserComponent->getCurrentMoveIndex() = bowserComponent->getCurrentMoveIndex() + 1;
        }
    } else if (bowserComponent->getCurrentMoveIndex() == 4) {
        if (bowserComponent->getlastStopTime() >= GLOBALS::FPS * 2) {
            bowserComponent->m_bowserMovements[0](entity);

            bowserComponent->getCurrentMoveIndex() = bowserComponent->getCurrentMoveIndex() + 1;
        }
    } else if (bowserComponent->getCurrentMoveIndex() == 5) {
        if (bowserComponent->getlastStopTime() >= GLOBALS::FPS * 3) {
            bowserComponent->m_bowserMovements[2](entity);

            bowserComponent->getCurrentMoveIndex() = bowserComponent->getCurrentMoveIndex() + 1;
        }
    }

    if (bowserComponent->getLastAttackTime() >= GLOBALS::FPS * 2) {
        auto hammerAmmount = rand() % 7 + 4;
        auto attackIndex = rand() % 2;
        bowserComponent->m_bowserAttacks[attackIndex](entity, hammerAmmount);
    }
}

void EnemySystem::performHammerBroActions(Entity *entity)
{
    auto *transform = entity->getComponent<TransformComponent>();
    auto texture = entity->getComponent<TextureComponent>();
    auto move = entity->getComponent<MovingComponent>();
    auto hammerBroComponent = entity->getComponent<HammerBroComponent>();

    if (!Camera::GetInstance().inCameraRange(transform) || entity->hasComponent<DeadComponent>()) {
        return;
    }

    auto player = m_world->findFirst<PlayerComponent>();
    auto playerTransform = player->getComponent<TransformComponent>();

    if (playerTransform->getPosition().x > transform->getPosition().x
        && !texture->isHorizontalFlipped()) {
        texture->setHorizontalFlipped(true);
    } else if (playerTransform->getPosition().x < transform->getPosition().x
               && texture->isHorizontalFlipped()) {
        texture->setHorizontalFlipped(false);
    }

    hammerBroComponent->getLastThrowTimer() += 1;
    hammerBroComponent->getLastJumpTimer() += 1;
    hammerBroComponent->getLastMoveTime() += 1;

    if (hammerBroComponent->getLastThrowTimer() == 2 * GLOBALS::FPS) {
        hammerBroComponent->getThrowHammer()(entity);
        if (hammerBroComponent->getLastJumpTimer()) {
            CommandScheduler::getInstance().addCommand(new DelayedCommand(
                [=]() {
                    move->getVelocity().y = -10.0f;
                    hammerBroComponent->getLastJumpTimer() = 0;
                },
                0.75f));
        }

        hammerBroComponent->getLastThrowTimer() = 0;
    }

    if (hammerBroComponent->getLastMoveTime() >= GLOBALS::FPS * 2.5f) {
        move->getVelocity().x *= -1;
        hammerBroComponent->getLastMoveTime() = 0;
    }
}

void EnemySystem::performLakituActions(Entity *entity)
{
    auto transform = entity->getComponent<TransformComponent>();
    auto texture = entity->getComponent<TextureComponent>();
    auto move = entity->getComponent<MovingComponent>();
    auto lakitu = entity->getComponent<LakituComponent>();

    if (!Camera::GetInstance().inCameraRange(transform)) {
        return;
    }

    auto player = m_world->findFirst<PlayerComponent>();
    auto playerTransform = player->getComponent<TransformComponent>();

    if (playerTransform->getPosition().x > transform->getPosition().x
        && !texture->isHorizontalFlipped()) {
        texture->setHorizontalFlipped(true);
    } else if (playerTransform->getPosition().x < transform->getPosition().x
               && texture->isHorizontalFlipped()) {
        texture->setHorizontalFlipped(false);
    }

    lakitu->getSideChangeTimer() += 1;

    if (lakitu->getSideChangeTimer() >= GLOBALS::FPS) {
        if (lakitu->getLakituSide() == Direction::Left) {
            lakitu->getLakituSide() = Direction::Right;
        } else {
            lakitu->getLakituSide() = Direction::Left;
        }
        lakitu->getSideChangeTimer() = 0;
    }

    auto flag = m_world->findFirst<FlagComponent>();

    // Lakitu stops harassing you if you're near the flag
    if (flag->getComponent<TransformComponent>()->getPosition().x - playerTransform->getPosition().x
        < 30 * GLOBALS::SCALED_CUBE_SIZE) {
        move->getVelocity().x = -4.0f;
        return;
    }

    // If not near the flag, move lakitu to the desired side of the screen
    if (lakitu->getLakituSide() == Direction::Right) {
        move->getVelocity().x = lakitu->getSpeedController()
                                    .calculate(transform->getPosition().x,
                                               Camera::GetInstance().getCameraCenterX()
                                                   + 6 * GLOBALS::SCALED_CUBE_SIZE);
    } else {
        move->getVelocity().x = lakitu->getSpeedController()
                                    .calculate(transform->getPosition().x,
                                               Camera::GetInstance().getCameraCenterX()
                                                   - 6 * GLOBALS::SCALED_CUBE_SIZE);
    }

    // Limits the speed to prevent lakitu from going zoooooooooooooooom
    if (std::abs(move->getVelocity().x) > 6.0f) {
        move->getVelocity().x = (move->getVelocity().x > 0.0f) ? 6.0f : -6.0f;
    }
}
