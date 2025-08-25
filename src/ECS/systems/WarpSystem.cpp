#include "WarpSystem.h"
#include "../../AABB.h"
#include "../../commands/CommandScheduler.h"
#include "../../commands/DelayedCommand.h"
#include "../../commands/VineCommand.h"
#include "../../commands/WarpCommand.h"
#include "../World.h"
#include "../components/FrozenComponent.h"
#include "../components/MovingComponent.h"
#include "../components/PlayerComponent.h"
#include "../components/TransformComponent.h"
#include "../components/VineComponent.h"

bool WarpSystem::warping = false;
bool WarpSystem::climbing = false;
bool WarpSystem::climbed = false;

WarpSystem::WarpSystem(GameWorld *gameWorld)
{
    m_scene = gameWorld;
}

void WarpSystem::onAddedToWorld(World *world)
{
    WarpSystem::setWarping(false);
    m_world = world;
}

void WarpSystem::tick(World *world)
{
    // Warp pipe checking
    world->find<WarpPipeComponent, TransformComponent>([&](Entity *entity) {
        auto *warpPipe = entity->getComponent<WarpPipeComponent>();

        Entity *player = world->findFirst<PlayerComponent>();

        // If colliding with pipe

        int x1 = entity->getComponent<TransformComponent>()->getPosition().x;
        int y1 = entity->getComponent<TransformComponent>()->getPosition().y;
        int w1 = entity->getComponent<TransformComponent>()->getHitbox().size.x;
        int h1 = entity->getComponent<TransformComponent>()->getHitbox().size.y;

        int x2 = player->getComponent<TransformComponent>()->getPosition().x
                 + player->getComponent<TransformComponent>()->getHitbox().position.x;
        int y2 = player->getComponent<TransformComponent>()->getPosition().y
                 + player->getComponent<TransformComponent>()->getHitbox().position.y;
        int w2 = player->getComponent<TransformComponent>()->getHitbox().size.x;
        int h2 = player->getComponent<TransformComponent>()->getHitbox().size.y;

        if (!AABB::isCollision(x1, y1, w1, h1, x2, y2, w2, h2) || WarpSystem::isWarping()) {
            return;
        }

        auto *playerMove = player->getComponent<MovingComponent>();

        switch (warpPipe->getInDirection()) {
        case Direction::Up: {
            if (m_up || playerMove->getVelocity().y < 0.0f) {
                warp(entity, player);
            }
        } break;
        case Direction::Down: {
            if (m_down || playerMove->getVelocity().y > 0.0f) {
                warp(entity, player);
            }
        } break;
        case Direction::Left: {
            if (m_left || playerMove->getVelocity().x < 0.0f) {
                warp(entity, player);
            }
        } break;
        case Direction::Right: {
            if (m_right || playerMove->getVelocity().x > 0.0f) {
                warp(entity, player);
            }
        } break;
        default:
            break;
        }
    });

    // Vine checking
    world->find<VineComponent, TransformComponent>([&](Entity *entity) {
        Entity *player = world->findFirst<PlayerComponent>();

        auto *playerTransform = player->getComponent<TransformComponent>();

        auto *playerMove = player->getComponent<MovingComponent>();

        if (!AABB::isTotalCollision(playerTransform, entity->getComponent<TransformComponent>())
            || (WarpSystem::isClimbing() && playerMove->getVelocity().y != 0.0f)) {
            return;
        }

        player->addComponent<CollisionExemptComponent>();
        player->addComponent<FrictionExemptComponent>();
        player->remove<GravityComponent>();

        player->getComponent<TextureComponent>()->setHorizontalFlipped(true);
        playerTransform->setLeft(entity->getComponent<TransformComponent>()->getRight()
                                 - GLOBALS::SCALED_CUBE_SIZE / 2);
        WarpSystem::setClimbing(true);
        PlayerSystem::setEnableInput(false);

        playerMove->getVelocity().x = playerMove->getAcceleration().x
            = playerMove->getAcceleration().y = playerMove->getVelocity().y = 0.0f;

        if (m_up) {
            climb(entity, player);
        }
    });

    // If the player is below the Y level where it gets teleported
    if (climbed) {
        Entity *player = world->findFirst<PlayerComponent, TransformComponent, MovingComponent>();
        auto *playerTransform = player->getComponent<TransformComponent>();
        auto *playerMove = player->getComponent<MovingComponent>();

        if (playerTransform->getTop() > m_teleportLevelY + GLOBALS::SCALED_CUBE_SIZE * 3) {
            player->addComponent<FrozenComponent>();

            WarpSystem::setClimbed(false);

            CommandScheduler::getInstance().addCommand(new DelayedCommand(
                [=]() {
                    player->remove<FrozenComponent>();

                    playerMove->getVelocity().x = playerMove->getAcceleration().x
                        = playerMove->getVelocity().y = playerMove->getAcceleration().y = 0.0f;

                    Camera::GetInstance().setCameraX(m_teleportCameraCoordinates.x);
                    Camera::GetInstance().setCameraY(m_teleportCameraCoordinates.y);
                    Camera::GetInstance().setCameraMaxX(m_teleportCameraMax);

                    //TextureManager::Get().SetBackgroundColor(teleportBackgroundColor);

                    //scene->setCurrentLevelType(teleportLevelType);
                    //scene->setLevelMusic(teleportLevelType);

                    playerTransform->setPosition(
                        sf::Vector2f(m_teleportPlayerCoordinates.x, m_teleportPlayerCoordinates.y));
                    //playerPosition->position = teleportPlayerCoordinates.convertTo<float>();

                    m_teleportPlayerCoordinates = sf::Vector2i(0, 0);
                    m_teleportLevelY = 0;
                    m_teleportCameraMax = 0;
                    m_teleportBackgroundColor = sf::Color(0, 0, 0);
                    m_teleportLevelType = LevelType::None;
                },
                2.0));
        }
    }
}

void WarpSystem::handleInput(const std::optional<sf::Event> &event)
{
    if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->scancode == sf::Keyboard::Scancode::Left) {
            m_left = 1;
        }
        if (keyPressed->scancode == sf::Keyboard::Scancode::Right) {
            m_right = 1;
        }
        if (keyPressed->scancode == sf::Keyboard::Scancode::Down) {
            m_down = 1;
        }
        if (keyPressed->scancode == sf::Keyboard::Scancode::Up) {
            m_up = 1;
        }
    }

    if (const auto *keyReleased = event->getIf<sf::Event::KeyReleased>()) {
        if (keyReleased->scancode == sf::Keyboard::Scancode::Left) {
            m_left = 0;
        }
        if (keyReleased->scancode == sf::Keyboard::Scancode::Right) {
            m_right = 0;
        }
        if (keyReleased->scancode == sf::Keyboard::Scancode::Down) {
            m_down = 0;
        }
        if (keyReleased->scancode == sf::Keyboard::Scancode::Up) {
            m_up = 0;
        }
    }
}

void WarpSystem::warp(Entity *pipe, Entity *player)
{
    if (WarpSystem::isWarping()) {
        return;
    }

    CommandScheduler::getInstance().addCommand(new WarpCommand(m_scene, m_world, pipe, player));
}

void WarpSystem::climb(Entity *vine, Entity *player)
{
    CommandScheduler::getInstance().addCommand(
        new VineCommand(m_scene, this, m_world, vine, player));
}
