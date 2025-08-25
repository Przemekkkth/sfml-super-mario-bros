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
#include "../../ECS/components/PlayerComponent.h"
#include "../../ECS/components/RightCollisionComponent.h"
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

    // Entity *flagSound(world->create());
    // flagSound->addComponent<SoundComponent>(SoundID::FLAG_RAISE);

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

            //m_gameWorld
            //m_world->getSystem<HUDSystem>()->setEnabled(false);
            auto hudSystem = m_world->getSystem<HUDSystem>();
            if (hudSystem->getGameTime() >= 0) {
                m_world->getSystem<HUDSystem>()->scoreCountdown();
            }
            //scene->scoreCountdown();

            //if (scene->scoreCountdownFinished() && nextLevelDelay == 0) {
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
    //self.scene:stopMusic()

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

            // local bridgeCollapseSound = Concord.entity(world)
            // bridgeCollapseSound:give('sound_component', SOUND_ID.BLOCK_BREAK)

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

            //Entity *bowserFall(world->create());
            //bowserFall->addComponent<SoundComponent>(SoundID::BOWSER_FALL);
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
                    //Entity *castleClear(world->create());

                    //castleClear->addComponent<SoundComponent>(SoundID::CASTLE_CLEAR);
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

            // CommandScheduler::getInstance().addCommand(new DelayedCommand(
            //     [=]() {
            //         sf::Vector2i nextLevel = m_gameWorld->getLevelData()
            //                                      .getNextLevel(); //scene->getLevelData().nextLevel;

            //         if (nextLevel != sf::Vector2i(0, 0)) {
            //             player->getComponent<TextureComponent>()->setVisible(false);
            //         } else {
            //             //Entity *winMusic(world->create());
            //             //winMusic->addComponent<MusicComponent>(MusicID::GAME_WON);
            //         }

            //         GameData::LEVEL = m_gameWorld->d.getNextLevel().x;
            //         GameData::SUB_LEVEL = m_gameWorld->d.getNextLevel().y;
            //         m_gameWorld->switchLevel();
            //     },
            //     3.0f));
        }),
        new DelayedCommand(
            [=]() {
                sf::Vector2i nextLevel = m_gameWorld->d.getNextLevel();

                if (nextLevel != sf::Vector2i(0, 0)) {
                    //player->getComponent<TextureComponent>()->setVisible(false);
                } else {
                    //Entity *winMusic(world->create());
                    //winMusic->addComponent<MusicComponent>(MusicID::GAME_WON);
                }

                GameData::LEVEL = m_gameWorld->d.getNextLevel().x;
                GameData::SUB_LEVEL = m_gameWorld->d.getNextLevel().y;
                m_gameWorld->switchLevel();
            },
            3.0f)}));

    /*

   local bridge
   for _, entity in ipairs(world:getEntities()) do
      if entity:has('bridge_component') then
         bridge = entity
         break
      end
   end

   bridge:give('timer_component', 
   function(entity)
      local bridgeComponent = entity.bridge_component
      bridgeComponent.connectedBridgeParts[#bridgeComponent.connectedBridgeParts]:give('destroy_delayed_component', 1)
      table.remove(bridgeComponent.connectedBridgeParts, #bridgeComponent.connectedBridgeParts)

      local bridgeCollapseSound = Concord.entity(world)
      bridgeCollapseSound:give('sound_component', SOUND_ID.BLOCK_BREAK)

      if #bridgeComponent.connectedBridgeParts == 0 then
         entity:remove('timer_component')
      end
   end, 5)

   CommandScheduler:addCommand(SequenceCommand({
      WaitUntilCommand(function() return not bridge:has('timer_component') end),
      RunCommand(
         function()
            bowser:remove('frozen_component')
            bowser:give('dead_component')
            local bowserFall = Concord.entity(world)
            bowserFall:give('sound_component', SOUND_ID.BOWSER_FALL)
         end
      ),
      -- Wait until bowser is not visible in the camera, then destroy the axe and move the player
      WaitUntilCommand(function() return not CameraInstance:inCameraRange(bowser.position) end),
      RunCommand(function()
         -- Play the castle clear sound in 0.325 seconds, this is separate from the sequence to
         -- avoid sequence interruption
         CommandScheduler:addCommand(
            DelayedCommand(
               function() 
                  local castleClear = Concord.entity(world)
                  castleClear:give('sound_component', SOUND_ID.CASTLE_CLEAR)
               end, 0.325))
         
         world:removeEntity(axe)
         playerMove.velocity.x = 3
         player:give('gravity_component')
         player:remove('frozen_component')
      end),
      -- Play the castle clear sound in 0.325 seconds, this is separate from the sequence to
      -- avoid sequence interruption
      WaitUntilCommand(function() return player:has('right_collision_component') end),
      RunCommand(function()
         FlagSystem.inSequence = false
         CommandScheduler:addCommand(DelayedCommand(
            function() 
               local nextLevel = self.scene:getLevelData().nextLevel
            
               if nextLevel.x ~= 0 and nextLevel.y ~= 0 then
                  player.texture:setVisible(false)
               else
                  local winMusic = Concord.entity(world)
                  winMusic:give('music_component', MUSIC_ID.GAME_WON)
               end
               self.scene:switchLevel(nextLevel.x, nextLevel.y)
            end, 5.0))
      end)
   }))

*/
}
