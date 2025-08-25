#ifndef VINECOMMAND_H
#define VINECOMMAND_H
#include "../Camera.h"
#include "../ECS/components/CollisionExemptComponent.h"
#include "../ECS/components/FrictionExemptComponent.h"
#include "../ECS/components/GravityComponent.h"
#include "../ECS/components/MovingComponent.h"
#include "../ECS/components/TextureComponent.h"
#include "../ECS/components/TransformComponent.h"
#include "../ECS/components/VineComponent.h"
#include "../ECS/systems/PlayerSystem.h"
#include "../ECS/systems/WarpSystem.h"
#include "../GameWorld.h"
#include "RunCommand.h"
#include "SequenceCommand.h"
#include "WaitUntilCommand.h"

class VineCommand : public SequenceCommand
{
public:
    VineCommand(GameWorld *scene, WarpSystem *warpSystem, World *world, Entity *vine, Entity *player)
    {
        auto *vineComponent = vine->getComponent<VineComponent>();

        auto *playerMove = player->getComponent<MovingComponent>();
        auto *playerTransform = player->getComponent<TransformComponent>();

        playerMove->getVelocity().y = -1.5f;

        warpSystem->setTeleportLevelY(vineComponent->getResetYvalue() * GLOBALS::SCALED_CUBE_SIZE);

        warpSystem->setTeleportPlayerCoordinates(
            sf::Vector2i(vineComponent->getResetTeleportLocation().x * GLOBALS::SCALED_CUBE_SIZE,
                         vineComponent->getResetTeleportLocation().y * GLOBALS::SCALED_CUBE_SIZE));

        warpSystem->setTeleportCameraCoordinates(
            sf::Vector2i((vineComponent->getResetTeleportLocation().x - 2)
                             * GLOBALS::SCALED_CUBE_SIZE,
                         (vineComponent->getResetTeleportLocation().y - 1)
                             * GLOBALS::SCALED_CUBE_SIZE));

        auto vineParts = vineComponent->getVineParts();

        addCommands(std::vector<Command *>{
            /* When the player is out of camera range, change the camera location */
            new WaitUntilCommand(
                [=]() -> bool { return !Camera::GetInstance().inCameraRange(playerTransform); }),
            new RunCommand([=]() {
                warpSystem->setTeleportCameraMax(Camera::GetInstance().getCameraMaxX());
                Camera::GetInstance().setCameraMaxX(vineComponent->getCameraMax()
                                                    * GLOBALS::SCALED_CUBE_SIZE);

                Camera::GetInstance().setCameraX(vineComponent->getCameraCoordinates().x
                                                 * GLOBALS::SCALED_CUBE_SIZE);
                Camera::GetInstance().setCameraY(vineComponent->getCameraCoordinates().y
                                                 * GLOBALS::SCALED_CUBE_SIZE);

                //warpSystem->setTeleportBackgroundColor(TextureManager::Get().getBackgroundColor());
                //TextureManager::Get().SetBackgroundColor(vineComponent->newBackgroundColor);

                //warpSystem->setTeleportLevelType(scene->getCurrentLevelType());
                //scene->setCurrentLevelType(vineComponent->newLevelType);
                //scene->setLevelMusic(vineComponent->newLevelType);
                // Moves the vines upwards
                for (auto *vinePiece : *vineParts) {
                    vinePiece->getComponent<MovingComponent>()->getVelocity().y = -1.0f;
                }
                // Sets mario's position to be at the bottom of the vine
                playerTransform->setTop(
                    vineParts->back()->getComponent<TransformComponent>()->getTop());
                playerTransform->setLeft(
                    vineParts->back()->getComponent<TransformComponent>()->getRight()
                    - GLOBALS::SCALED_CUBE_SIZE / 2);
            }),
            /* Wait until the vine has fully moved up, and then stop the vines from growing more */
            new WaitUntilCommand([=]() -> bool {
                return vineParts->at(0)->getComponent<TransformComponent>()->getTop()
                       <= (vineComponent->getTeleportCoordinates().y * GLOBALS::SCALED_CUBE_SIZE)
                              - (GLOBALS::SCALED_CUBE_SIZE * 4);
            }),
            new RunCommand([=]() {
                for (Entity *vinePiece : *vineParts) {
                    vinePiece->getComponent<MovingComponent>()->getVelocity().y = 0.0f;
                    vinePiece->remove<VineComponent>();
                }
            }),
            /* Wait until the player has climbed to the top of the vine, then end the sequence */
            new WaitUntilCommand([=]() -> bool {
                return playerTransform->getBottom()
                       <= vineParts->at(1)->getComponent<TransformComponent>()->getBottom();
            }),
            new RunCommand([=]() {
                // Moves the player away from the vine
                playerTransform->setLeft(
                    vineParts->front()->getComponent<TransformComponent>()->getRight());

                player->getComponent<TextureComponent>()->setHorizontalFlipped(false);
                player->addComponent<GravityComponent>();
                player->remove<FrictionExemptComponent>();
                player->remove<CollisionExemptComponent>();

                PlayerSystem::setEnableInput(true);
                WarpSystem::setClimbing(false);

                WarpSystem::setClimbed(true);
            })});
    }

    virtual void execute() override { SequenceCommand::execute(); }

    virtual bool isFinished() override { return SequenceCommand::isFinished(); }
};

#endif // VINECOMMAND_H
