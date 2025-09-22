#include "FlagSystem.h"
#include "../../AABB.h"
#include "../../Camera.h"
#include "../../ECS/components/AxeComponent.h"
#include "../../ECS/components/BottomCollisionComponent.h"
#include "../../ECS/components/BowserComponent.h"
#include "../../ECS/components/BridgeChainComponent.h"
#include "../../ECS/components/BridgeComponent.h"
#include "../../ECS/components/DeadComponent.h"
#include "../../ECS/components/DestroyDelayedComponent.h"
#include "../../ECS/components/FlagComponent.h"
#include "../../ECS/components/FlagPoleComponent.h"
#include "../../ECS/components/FrictionExemptComponent.h"
#include "../../ECS/components/FrozenComponent.h"
#include "../../ECS/components/GravityComponent.h"
#include "../../ECS/components/LeftCollisionComponent.h"
#include "../../ECS/components/MovingComponent.h"
#include "../../ECS/components/MusicComponent.h"
#include "../../ECS/components/PlayerComponent.h"
#include "../../ECS/components/RightCollisionComponent.h"
#include "../../ECS/components/SoundComponent.h"
#include "../../ECS/components/TextureComponent.h"
#include "../../ECS/components/TimerComponent.h"
#include "../../ECS/components/TransformComponent.h"
#include "../../GameData.h"
#include "../../GameWorld.h"
#include "../../commands/Command.h"
#include "../../commands/CommandScheduler.h"
#include "../../commands/DelayedCommand.h"
#include "../../commands/RunCommand.h"
#include "../../commands/SequenceCommand.h"
#include "../../commands/WaitCommand.h"
#include "../../commands/WaitUntilCommand.h"
#include "../Entity.h"
#include "../World.h"
#include "HUDSystem.h"
#include "MusicSystem.h"
#include "PlayerSystem.h"

bool FlagSystem::inSequence = false;
bool FlagSystem::climbing = false;

void FlagSystem::setClimbing(bool on)
{
    climbing = on;
}

bool FlagSystem::isClimbing()
{
    return climbing;
}

FlagSystem::FlagSystem(GameWorld *gameWorld)
    : m_gameWorld(gameWorld)
{
    FlagSystem::climbing = false;
}

void FlagSystem::onAddedToWorld(World *world)
{
    m_world = world;
}

void FlagSystem::tick(World *world)
{
    world->find<FlagPoleComponent>([&](Entity *entity) {
        Entity *player = world->findFirst<PlayerComponent>();

        if (!AABB::isTotalCollision(entity->getComponent<TransformComponent>(),
                                    player->getComponent<TransformComponent>())
            || FlagSystem::climbing) {
            return;
        }

        climbFlag();
    });

    world->find<AxeComponent>([&](Entity *entity) {
        Entity *player = world->findFirst<PlayerComponent>();

        if (!AABB::isTotalCollision(entity->getComponent<TransformComponent>(),
                                    player->getComponent<TransformComponent>())) {
            return;
        }

        hitAxe();
    });
}

void FlagSystem::climbFlag()
{
    if (inSequence) {
        return;
    }

    auto player = m_world->findFirst<PlayerComponent>();
    auto *playerMove = player->getComponent<MovingComponent>();
    auto *playerTransform = player->getComponent<TransformComponent>();

    auto flag = m_world->findFirst<FlagComponent>();
    auto flagMove = flag->getComponent<MovingComponent>();

    PlayerSystem::setEnableInput(false);

    FlagSystem::climbing = true;

    playerTransform->setLeft(flag->getComponent<TransformComponent>()->getLeft());

    playerMove->getVelocity().x = 0.0f;
    playerMove->getAcceleration().x = 0.0f;
    playerMove->getAcceleration().y = 0.0f;
    playerMove->getVelocity().y = 4.0f;

    flagMove->getVelocity().y = 4.0f;

    // scene->stopTimer();
    // scene->stopMusic();
    m_world->getSystem<MusicSystem>()->stop();

    Entity *flagSound(m_world->create());
    flagSound->addComponent<SoundComponent>(SoundID::FlagRaise);

    player->remove<GravityComponent>();
    player->addComponent<FrictionExemptComponent>();

    inSequence = true;

    CommandScheduler::getInstance().addCommand(new SequenceCommand(std::vector<Command *>{
        /* Move to the other side of the flag */
        new WaitUntilCommand([=]() -> bool {
            return player->hasComponent<BottomCollisionComponent>()
                   && flag->hasComponent<BottomCollisionComponent>();
        }),
        new RunCommand([=]() {
            playerMove->getVelocity().y = 0.0f;
            flagMove->getVelocity().y = 0.0f;

            player->getComponent<TextureComponent>()->setHorizontalFlipped(true);
            playerTransform->move(sf::Vector2f{34.0f, 0.0f});
        }),
        new WaitCommand(0.6f),
        /* Move towards the castle */
        new RunCommand([=]() {
            FlagSystem::setClimbing(false);

            Camera::GetInstance().setCameraFrozen(false);

            player->addComponent<GravityComponent>();

            playerMove->getVelocity().x = 2.0f;
            player->getComponent<TextureComponent>()->setHorizontalFlipped(false);
        }),
        /* Wait until the player hits a solid block */
        new WaitUntilCommand(
            [=]() -> bool { return player->hasComponent<RightCollisionComponent>(); }),
        new WaitUntilCommand([=]() -> bool {
            playerMove->getVelocity().x = 0.0f;
            static int nextLevelDelay = (int) std::round(GLOBALS::FPS * 4.5);

            if (nextLevelDelay > 0) {
                nextLevelDelay--;
            }

            auto hudSystem = m_world->getSystem<HUDSystem>();
            if (hudSystem->getGameTime() >= 0) {
                m_world->getSystem<HUDSystem>()->scoreCountdown();
            }

            if (nextLevelDelay == 0 && hudSystem->scoreCountdownFinished()) {
                nextLevelDelay = (int) std::round(GLOBALS::FPS * 4.5);
                return true;
            }
            return false;
        }),
        new RunCommand([=]() mutable {
            player->getComponent<TextureComponent>()->setVisible(false);

            inSequence = false;

            GameData::LEVEL = m_gameWorld->d.getNextLevel().x;
            GameData::SUB_LEVEL = m_gameWorld->d.getNextLevel().y;
            CommandScheduler::getInstance().addCommand(
                new DelayedCommand([=]() { m_gameWorld->switchLevel(); }, 2.0));
        })}));
}

void FlagSystem::hitAxe()
{
    if (inSequence) {
        return;
    }

    PlayerSystem::setEnableInput(false);
    auto axe = m_world->findFirst<AxeComponent>();
    auto player = m_world->findFirst<PlayerComponent>();
    auto playerMove = player->getComponent<MovingComponent>();
    playerMove->getVelocity() = sf::Vector2f(0.0f, 0.0f);
    playerMove->getAcceleration() = sf::Vector2f(0.0f, 0.0f);

    //self.scene:stopTimer()
    m_world->getSystem<MusicSystem>()->stop();

    player->remove<GravityComponent>();
    player->addComponent<FrictionExemptComponent>();
    player->addComponent<FrozenComponent>();

    inSequence = true;
    auto bridgeChain = m_world->findFirst<BridgeChainComponent>();
    m_world->destroy(bridgeChain);

    auto bowser = m_world->findFirst<BowserComponent>();
    bowser->addComponent<FrozenComponent>();

    auto bridge = m_world->findFirst<BridgeComponent>();
    bridge->addComponent<TimerComponent>(
        [=](Entity *entity) {
            auto bridgeComponent = entity->getComponent<BridgeComponent>();
            bridgeComponent->getConnectedBridgeParts()
                .at(bridgeComponent->getConnectedBridgeParts().size() - 1)
                ->addComponent<DestroyDelayedComponent>(1);
            bridgeComponent->getConnectedBridgeParts().pop_back();

            auto bridgeCollapseSound = m_world->create();
            bridgeCollapseSound->addComponent<SoundComponent>(SoundID::BlockBreak);

            if (bridgeComponent->getConnectedBridgeParts().size() == 0) {
                entity->remove<TimerComponent>();
            }
        },
        5);

    CommandScheduler::getInstance().addCommand(new SequenceCommand(std::vector<Command *>{
        /* Wait until the bridge is done collapsing, then make bowser fall */
        new WaitUntilCommand([=]() -> bool { return !bridge->hasComponent<TimerComponent>(); }),
        new RunCommand([=]() {
            bowser->remove<FrozenComponent>();
            bowser->addComponent<DeadComponent>();

            Entity *bowserFall(m_world->create());
            bowserFall->addComponent<SoundComponent>(SoundID::BowserFall);
        }),
        /* Wait until bowser is not visible in the camera, then destroy the axe and move the player
        */
        new WaitUntilCommand([bowser]() -> bool {
            return !Camera::GetInstance().inCameraRange(bowser->getComponent<TransformComponent>());
        }),
        new RunCommand([=]() {
            // Play the castle clear sound in 0.325 seconds, this is separate from the sequence to
            // avoid sequence interruption
            CommandScheduler::getInstance().addCommand(new DelayedCommand(
                [=]() {
                    Entity *castleClear(m_world->create());

                    castleClear->addComponent<SoundComponent>(SoundID::CastleClear);
                },
                0.325f));

            m_world->destroy(axe);

            playerMove->getVelocity().x = 3.0f;

            player->addComponent<GravityComponent>();

            player->remove<FrozenComponent>();
        }),
        /* Wait until mario runs into a block, then stop and switch to the next level */
        new WaitUntilCommand(
            [player]() -> bool { return player->hasComponent<RightCollisionComponent>(); }),
        new RunCommand([=]() mutable {
            inSequence = false;
        }),
        new DelayedCommand(
            [=]() {
                sf::Vector2i nextLevel = m_gameWorld->d.getNextLevel();

                if (nextLevel != sf::Vector2i(0, 0)) {
                    GameData::LEVEL = m_gameWorld->d.getNextLevel().x;
                    GameData::SUB_LEVEL = m_gameWorld->d.getNextLevel().y;
                    m_gameWorld->switchLevel();
                } else {
                    Entity *winMusic(m_world->create());
                    winMusic->addComponent<MusicComponent>(MusicID::GameWon);
                    m_gameWorld->switchToMenu();
                }
            },
            3.0f)}));
}
