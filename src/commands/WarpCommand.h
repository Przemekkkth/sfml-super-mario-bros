#ifndef WARPCOMMAND_H
#define WARPCOMMAND_H
#include "../Camera.h"
#include "../ECS/Entity.h"
#include "../ECS/World.h"
#include "../ECS/components/CollisionExemptComponent.h"
#include "../ECS/components/DeadComponent.h"
#include "../ECS/components/FrictionExemptComponent.h"
#include "../ECS/components/GravityComponent.h"
#include "../ECS/components/MovingComponent.h"
#include "../ECS/components/ParticleComponent.h"
#include "../ECS/components/PiranhaPlantComponent.h"
#include "../ECS/components/SoundComponent.h"
#include "../ECS/components/TimerComponent.h"
#include "../ECS/components/TransformComponent.h"
#include "../ECS/components/WaitUntilComponent.h"
#include "../ECS/components/WarpPipeComponent.h"
#include "../ECS/systems/PlayerSystem.h"
#include "../ECS/systems/WarpSystem.h"
#include "../GameData.h"
#include "../GameWorld.h"
#include "RunCommand.h"
#include "SequenceCommand.h"
#include "WaitUntilCommand.h"

class WarpCommand : public SequenceCommand
{
public:
    WarpCommand(GameWorld *scene, World *world, Entity *pipe, Entity *player)
    {
        if (player->hasAny<ParticleComponent, DeadComponent>()) {
            addCommands(std::vector<Command *>{new RunCommand([]() {})});
            return;
        }

        auto *warpPipe = pipe->getComponent<WarpPipeComponent>();

        auto *playerTransform = player->getComponent<TransformComponent>();
        auto *playerMove = player->getComponent<MovingComponent>();

        sf::Vector2f pipeLocation = pipe->getComponent<TransformComponent>()->getPosition();
        sf::Vector2i teleportLocation = warpPipe->getPlayerLocation();

        Camera::GetInstance().setCameraLeft(warpPipe->getCameraLocation().x);

        WarpSystem::setWarping(true);
        PlayerSystem::setEnableInput(false);

        // scene->stopMusic();

        Entity *pipeSound(world->create());
        pipeSound->addComponent<SoundComponent>(SoundID::Pipe);

        player->addComponent<CollisionExemptComponent>();
        player->addComponent<FrictionExemptComponent>();
        player->remove<GravityComponent>();

        addCommands(std::vector<Command *>{
            new RunCommand([=]() {
                // Set the player's speed to go in the pipe
                switch (warpPipe->getInDirection()) {
                case Direction::Up: {
                    playerMove->getVelocity().y = -1.0f;
                    playerMove->getAcceleration().y = 0.0f;
                    playerMove->getVelocity().x = playerMove->getAcceleration().x = 0.0f;
                } break;
                case Direction::Down: {
                    playerMove->getVelocity().y = 1.0f;
                    playerMove->getAcceleration().y = 0.0f;
                    playerMove->getVelocity().x = playerMove->getAcceleration().x = 0.0f;
                } break;
                case Direction::Left: {
                    playerMove->getVelocity().x = -1.0f;
                    playerMove->getAcceleration().x = 0.0f;
                    playerMove->getVelocity().y = playerMove->getAcceleration().y = 0.0f;
                } break;
                case Direction::Right: {
                    playerMove->getVelocity().x = 1.0f;
                    playerMove->getAcceleration().x = 0.0f;
                    playerMove->getVelocity().y = playerMove->getAcceleration().y = 0.0f;
                } break;
                }
            }),
            /* Enter the pipe */
            new WaitUntilCommand([=]() -> bool {
                switch (warpPipe->getInDirection()) {
                case Direction::Up:
                    return player->getComponent<TransformComponent>()->getBottom()
                           < pipeLocation.y - 32.0f;
                    break;
                case Direction::Down:
                    return player->getComponent<TransformComponent>()->getTop()
                           > pipeLocation.y + 32.0f;
                    break;
                case Direction::Left:
                    return player->getComponent<TransformComponent>()->getRight()
                           < pipeLocation.x - 32.0f;
                    break;
                case Direction::Right:
                    return player->getComponent<TransformComponent>()->getLeft()
                           > pipeLocation.x + 32.0f;
                    break;
                default:
                    return false;
                    break;
                }
            }),
            /* Teleport or go to new level */
            new RunCommand([=]() {
                if (warpPipe->getNewLevel() != sf::Vector2i(0, 0)) {
                    Camera::GetInstance().setCameraFrozen(false);

                    player->remove<CollisionExemptComponent, FrictionExemptComponent>();
                    player->addComponent<GravityComponent>();

                    //TextureManager::Get().SetBackgroundColor(BackgroundColor::BLACK);

                    player->remove<WaitUntilComponent>();

                    GameData::LEVEL = warpPipe->getNewLevel().x;
                    GameData::SUB_LEVEL = warpPipe->getNewLevel().y;
                    scene->switchLevel();
                    return;
                }

                // Puts the piranha plants back in the pipe
                world->find<PiranhaPlantComponent>([&](Entity *piranha) {
                    auto *piranhaComponent = piranha->getComponent<PiranhaPlantComponent>();

                    if (!piranhaComponent->getInPipe()) {
                        // piranha->getComponent<PositionComponent>()->position.y
                        //     = piranhaComponent->pipeCoordinates.y;
                        piranha->getComponent<TransformComponent>()->setTop(
                            piranhaComponent->getPipeCoordinates().y);
                        piranha->getComponent<PiranhaPlantComponent>()->getInPipe() = true;
                        piranha->getComponent<MovingComponent>()->getVelocity().y = 0.0f;

                        piranha->getComponent<TimerComponent>()->reset();
                        piranha->remove<WaitUntilComponent>();
                    }
                });

                scene->setUnderwater(warpPipe->getLevelType() == LevelType::Underwater);
                scene->setLevelMusic(warpPipe->getLevelType());

                Camera::GetInstance().setCameraX(warpPipe->getCameraLocation().x
                                                 * GLOBALS::SCALED_CUBE_SIZE);
                Camera::GetInstance().setCameraY(warpPipe->getCameraLocation().y
                                                 * GLOBALS::SCALED_CUBE_SIZE);
                Camera::GetInstance().updateCameraMin();
                Camera::GetInstance().setCameraFrozen(warpPipe->getCameraFreeze());

                //TextureManager::Get().SetBackgroundColor(warpPipe->backgroundColor);
                scene->d.setBackgroundColor(warpPipe->getBackgroundColor());

                switch (warpPipe->getOutDirection()) {
                case Direction::Up: {
                    playerTransform->setPosition(
                        sf::Vector2f(teleportLocation.x * GLOBALS::SCALED_CUBE_SIZE,
                                     (teleportLocation.y + 1) * GLOBALS::SCALED_CUBE_SIZE));

                    playerMove->getVelocity().y = -1.0f;
                    playerMove->getVelocity().x = playerMove->getAcceleration().x = 0.0f;
                } break;
                case Direction::Down: {
                    playerTransform->setPosition(
                        sf::Vector2f(teleportLocation.x * GLOBALS::SCALED_CUBE_SIZE,
                                     (teleportLocation.y - 1) * GLOBALS::SCALED_CUBE_SIZE));

                    playerMove->getVelocity().y = 1.0f;
                    playerMove->getVelocity().x = playerMove->getAcceleration().x = 0.0f;
                    break;
                case Direction::Left:
                    playerTransform->setPosition(
                        sf::Vector2f((teleportLocation.x + 1) * GLOBALS::SCALED_CUBE_SIZE,
                                     teleportLocation.y * GLOBALS::SCALED_CUBE_SIZE));

                    playerMove->getVelocity().x = -1.0f;
                    playerMove->getVelocity().y = playerMove->getAcceleration().y = 0.0;
                    break;
                case Direction::Right:
                    playerTransform->setPosition(
                        sf::Vector2f((teleportLocation.x - 1) * GLOBALS::SCALED_CUBE_SIZE,
                                     teleportLocation.y * GLOBALS::SCALED_CUBE_SIZE));

                    playerMove->getVelocity().x = 1.0f;
                    playerMove->getVelocity().y = playerMove->getAcceleration().y = 0.0f;
                    break;
                case Direction::None:
                    playerTransform->setPosition(
                        sf::Vector2f(teleportLocation.x * GLOBALS::SCALED_CUBE_SIZE,
                                     teleportLocation.y * GLOBALS::SCALED_CUBE_SIZE));
                    playerMove->getVelocity().x = playerMove->getAcceleration().x
                        = playerMove->getVelocity().y = playerMove->getAcceleration().y = 0.0f;
                    break;
                default:
                    break;
                }
                }
            })});

        if (warpPipe->getNewLevel() == sf::Vector2i(0, 0)) {
            addCommands(std::vector<Command *>{
                new WaitUntilCommand([=]() -> bool {
                    switch (warpPipe->getOutDirection()) {
                    case Direction::Up:
                        return playerTransform->getBottom()
                               < teleportLocation.y * GLOBALS::SCALED_CUBE_SIZE;
                        break;
                    case Direction::Down:
                        return playerTransform->getTop()
                               > teleportLocation.y * GLOBALS::SCALED_CUBE_SIZE;
                        break;
                    case Direction::Left:
                        return playerTransform->getRight()
                               < teleportLocation.x * GLOBALS::SCALED_CUBE_SIZE;
                        break;
                    case Direction::Right:
                        return playerTransform->getLeft()
                               > teleportLocation.x * GLOBALS::SCALED_CUBE_SIZE;
                        break;
                    default:
                        return true;
                        break;
                    }
                }),
                new RunCommand([=]() {
                    WarpSystem::setWarping(false);
                    PlayerSystem::setEnableInput(true);
                    //PlayerSystem::setGameStart(false);

                    playerMove->getVelocity().x = playerMove->getAcceleration().x
                        = playerMove->getVelocity().y = playerMove->getAcceleration().y = 0.0f;

                    player->addComponent<GravityComponent>();
                    player->remove<CollisionExemptComponent>();
                    player->remove<FrictionExemptComponent>();
                })});
        }
    }

    virtual void execute() override { SequenceCommand::execute(); }
    virtual bool isFinished() override { return SequenceCommand::isFinished(); }
};

#endif // WARPCOMMAND_H
