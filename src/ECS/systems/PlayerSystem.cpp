#include "PlayerSystem.h"
#include "../../AABB.h"
#include "../../Camera.h"
#include "../../Game.h"
#include "../../Map.h"
#include "../World.h"
#include "../components/AddLivesComponent.h"
#include "../components/AddScoreComponent.h"
#include "../components/AnimationComponent.h"
#include "../components/BlockBumpComponent.h"
#include "../components/BottomCollisionComponent.h"
#include "../components/BumpableComponent.h"
#include "../components/CallbackComponent.h"
#include "../components/CollectibleComponent.h"
#include "../components/CollisionExemptComponent.h"
#include "../components/CreateFloatingTextComponent.h"
#include "../components/CrushableComponent.h"
#include "../components/CrushedComponent.h"
#include "../components/DeadComponent.h"
#include "../components/DestroyDelayedComponent.h"
#include "../components/DestroyOutsideCameraComponent.h"
#include "../components/DestructibleComponent.h"
#include "../components/EndingBlinkComponent.h"
#include "../components/EnemyComponent.h"
#include "../components/EnemyDestroyedComponent.h"
#include "../components/FrictionExemptComponent.h"
#include "../components/FrozenComponent.h"
#include "../components/GravityComponent.h"
#include "../components/InvisibleBlockComponent.h"
#include "../components/LeftCollisionComponent.h"
#include "../components/MovingComponent.h"
#include "../components/MovingPlatformComponent.h"
#include "../components/MysteryBoxComponent.h"
#include "../components/ParticleComponent.h"
#include "../components/PlayerComponent.h"
#include "../components/ProjectileComponent.h"
#include "../components/RightCollisionComponent.h"
#include "../components/SoundComponent.h"
#include "../components/SpritesheetComponent.h"
#include "../components/TextureComponent.h"
#include "../components/TileComponent.h"
#include "../components/TopCollisionComponent.h"
#include "../components/TrampolineComponent.h"
#include "../components/TransformComponent.h"
#include "../components/WaitUntilComponent.h"
#include "FlagSystem.h"
#include "WarpSystem.h"

#include "HUDSystem.h"

bool PlayerSystem::m_enabled = false;

bool PlayerSystem::isInputEnabled()
{
    return m_enabled;
}

void PlayerSystem::setEnableInput(bool on)
{
    m_enabled = on;
}

PlayerSystem::PlayerSystem(LevelData levelData)
{
    m_levelData = levelData;
}

void PlayerSystem::onAddedToWorld(World *world)
{
    m_world = world;
    m_mario = world->create();
    auto startCoordinates = m_levelData.getPlayerStart();
    m_mario->addComponent<TransformComponent>(sf::Vector2f(startCoordinates)
                                              * float(GLOBALS::SCALED_CUBE_SIZE));
    m_mario->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::PlayerSprteSheet));
    m_mario->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                                GLOBALS::ORIGINAL_CUBE_SIZE,
                                                1,
                                                9,
                                                0,
                                                GLOBALS::ORIGINAL_CUBE_SIZE,
                                                GLOBALS::ORIGINAL_CUBE_SIZE,
                                                Map::PlayerIDCoordinates.at(0));
    m_mario->addComponent<MovingComponent>(sf::Vector2f{0.0f, 0.0f}, sf::Vector2f{0.0f, 0.0f});
    m_mario->addComponent<PlayerComponent>();

    //reset
    Camera::GetInstance().setCameraX(m_levelData.getCameraStart().x * GLOBALS::SCALED_CUBE_SIZE);
    //Camera::GetInstance().setCameraX(0);
    Camera::GetInstance().setCameraY(m_levelData.getCameraStart().y * GLOBALS::SCALED_CUBE_SIZE);

    m_mario->remove<FrozenComponent>();

    if (m_levelData.getLevelType() != LevelType::StartUnderground) {
        m_mario->addComponent<GravityComponent>();

        m_mario->remove<FrictionExemptComponent>();
        m_mario->remove<CollisionExemptComponent>();

        Camera::GetInstance().setCameraFrozen(false);
        m_enabled = true;
        //PlayerSystem::enableInput(true);
    } else {
        Camera::GetInstance().setCameraFrozen(true);

        //PlayerSystem::setGameStart(true);
        //PlayerSystem::enableInput(false);
        m_enabled = false;

        m_mario->addComponent<FrictionExemptComponent>();
        m_mario->addComponent<CollisionExemptComponent>();

        m_mario->remove<GravityComponent>();

        m_mario->getComponent<MovingComponent>()->getVelocity().x = 1.6f;
    }

    m_underwaterControllerX = PIDController(0.20, 0, 0.02, 60);
}

void PlayerSystem::tick(World *world)
{
    m_xDir = m_right - m_left;
    auto move = m_mario->getComponent<MovingComponent>();
    auto transform = m_mario->getComponent<TransformComponent>();
    const float EPSILON = 1e-6f;

    if (FlagSystem::isClimbing()) {
        m_currentState = AnimationState::Sliding;
        setCurrentState(m_currentState);
        updateCamera();
        return;
    }

    if (WarpSystem::isWarping()) {
        if (move->getVelocity().x != 0.0f) {
            m_currentState = AnimationState::Walking;
        }
        if (move->getVelocity().x == 0.0f || move->getVelocity().y != 0.0f) {
            m_currentState = AnimationState::Standing;
        }

        setCurrentState(m_currentState);
        updateCamera();
        return;
    }

    if (WarpSystem::isClimbing()) {
        if (move->getVelocity().y != 0.0f) {
            m_currentState = AnimationState::Climbing;
        } else {
            m_currentState = AnimationState::Sliding;
        }
        setCurrentState(m_currentState);
        updateCamera();
        return;
    }

    if (!isInputEnabled()) {
        if (m_levelData.getLevelType() == LevelType::StartUnderground) {
            move->getVelocity().x = 1.6f;
        }

        if (move->getVelocity().x != 0.0f && move->getVelocity().y == 0.0f) {
            m_currentState = AnimationState::Walking;
        } else if (move->getVelocity().y != 0.0f) {
            m_currentState = AnimationState::Jumping;
        } else {
            m_currentState = AnimationState::Standing;
        }

        setCurrentState(m_currentState);
        updateCamera();
        return;
    }

    if (m_currentState != AnimationState::Gameover) {
        checkGameTime();
    }

    //return;
    if (m_currentState != AnimationState::Gameover) {
        if (m_underwater) {
            updateWaterVelocity();
        } else if (m_mario->hasComponent<BottomCollisionComponent>()) {
            updateGroundVelocity();
        } else {
            updateAirVelocity();
        }
    } else {
        setCurrentState(AnimationState::Gameover);
        return;
    }

    if (m_holdFire) {
        m_currentState = AnimationState::Launch_Fireball;
    }

    if (transform->getPosition().y >= Camera::GetInstance().getCameraY() + GLOBALS::WINDOW_HEIGHT
                                          + GLOBALS::SCALED_CUBE_SIZE
        && !m_mario->hasComponent<FrozenComponent>() && !WarpSystem::hasClimbed()) {
        onGameOver(true);
        return;
    }

    checkCollisionWithMovingPlatform();
    checkCollisionWithTrampoline();
    launchFireballs();

    checkEnemyCollisions();
    checkCollisionWithProjectile();
    checkCollisionWithBreakBlock();
    checkCollisionWithPowerUp();

    updateCamera();
    // Updates the textures for whichever state the player is currently in
    setCurrentState(m_currentState);
    //This resets the collision/jumping states to avoid conflicts during the next game tick
    if (m_mario->hasComponent<TopCollisionComponent>()) {
        m_mario->remove<TopCollisionComponent>();
    }
    if (m_mario->hasComponent<RightCollisionComponent>()) {
        m_mario->remove<RightCollisionComponent>();
    }
    if (m_mario->hasComponent<BottomCollisionComponent>()) {
        m_mario->remove<BottomCollisionComponent>();
    }
    if (m_mario->hasComponent<LeftCollisionComponent>()) {
        m_mario->remove<LeftCollisionComponent>();
    }
}

void PlayerSystem::handleInput(const std::optional<sf::Event> &event)
{
    if (!isInputEnabled()) {
        m_left = 0;
        m_right = 0;
        m_duck = 0;
        m_jump = 0;
        m_launchFireball = 0;
        m_running = 0;
        m_jumpHeld = 0;
    }

    if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->scancode == sf::Keyboard::Scancode::Left) {
            m_left = 1;
        }
        if (keyPressed->scancode == sf::Keyboard::Scancode::Right) {
            m_right = 1;
        }
        if (keyPressed->scancode == sf::Keyboard::Scancode::Down) {
            m_duck = 1;
        }
        if (keyPressed->scancode == sf::Keyboard::Scancode::Space) {
            m_jump = 1;
            m_jumpHeld = 1;
        }
        if (keyPressed->scancode == sf::Keyboard::Scancode::Q && !m_holdFire) {
            m_launchFireball = 1;
        }
        if (keyPressed->scancode == sf::Keyboard::Scancode::LShift) {
            m_running = 1;
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
            m_duck = 0;
        }
        if (keyReleased->scancode == sf::Keyboard::Scancode::Space) {
            m_jump = 0;
            m_jumpHeld = 0;
        }
        if (keyReleased->scancode == sf::Keyboard::Scancode::Q) {
            m_launchFireball = 0;
        }
        if (keyReleased->scancode == sf::Keyboard::Scancode::LShift) {
            m_running = 0;
        }
    }
}

void PlayerSystem::handleInput() {}

bool PlayerSystem::PlayerSystem::isSmallMario()
{
    return m_mario->getComponent<PlayerComponent>()->getPlayerState() == PlayerState::SmallMario;
}

bool PlayerSystem::PlayerSystem::isSuperMario()
{
    return m_mario->getComponent<PlayerComponent>()->getPlayerState() == PlayerState::SuperMario;
}

bool PlayerSystem::PlayerSystem::isFireMario()
{
    return m_mario->getComponent<PlayerComponent>()->getPlayerState() == PlayerState::FireMario;
}

bool PlayerSystem::PlayerSystem::isSuperStar()
{
    return m_mario->getComponent<PlayerComponent>()->isSuperStar();
}

bool PlayerSystem::isGameOver()
{
    return m_currentState == AnimationState::Gameover;
}

void PlayerSystem::setCurrentState(AnimationState newState)
{
    auto spritesheet = m_mario->getComponent<SpritesheetComponent>();

    if (m_mario->hasComponent<FrozenComponent>()) {
        return;
    }

    if (newState == AnimationState::Standing) {
        if (m_mario->hasComponent<AnimationState>()) {
            m_mario->remove<AnimationState>();
        }

        if (isFireMario()) {
            spritesheet->setSpritesheetCoordinates(Map::PlayerIDCoordinates.at(225));
        } else if (isSuperMario()) {
            spritesheet->setSpritesheetCoordinates(Map::PlayerIDCoordinates.at(25));
        } else {
            spritesheet->setSpritesheetCoordinates(Map::PlayerIDCoordinates.at(0));
        }
    } else if (newState == AnimationState::Walking) {
        std::vector<int> fireFrameIDs{227, 228, 229};
        std::vector<int> superFrameIDs{27, 28, 29};
        std::vector<int> normalFrameIDs{2, 3, 4};
        const int frameCount = 3;
        const int framesPerSecond = 8;
        if (!m_mario->hasComponent<AnimationComponent>()) {
            if (isFireMario()) {
                m_mario->addComponent<AnimationComponent>(fireFrameIDs,
                                                          framesPerSecond,
                                                          Map::PlayerIDCoordinates);
            } else if (isSuperMario()) {
                m_mario->addComponent<AnimationComponent>(superFrameIDs,
                                                          framesPerSecond,
                                                          Map::PlayerIDCoordinates);
            } else {
                m_mario->addComponent<AnimationComponent>(normalFrameIDs,
                                                          framesPerSecond,
                                                          Map::PlayerIDCoordinates);
                return;
            }
        }

        auto animation = m_mario->getComponent<AnimationComponent>();
        if (m_mario->getComponent<AnimationComponent>()->getFrameIDs() != superFrameIDs
            && m_mario->getComponent<AnimationComponent>()->getFrameIDs() != normalFrameIDs
            && m_mario->getComponent<AnimationComponent>()->getFrameIDs() != fireFrameIDs) {
            //If the player already has an animation but it is not the correct one

            animation->setFrameCount(frameCount);
            animation->setRepeated(true);
            if (isFireMario()) {
                animation->setFrameIDs(fireFrameIDs);
            } else if (isSuperMario()) {
                animation->setFrameIDs(superFrameIDs);
            } else {
                animation->setFrameIDs(normalFrameIDs);
            }
        }

        if (m_running > 0) {
            animation->setFramesPerSecond(12);
        } else {
            animation->setFramesPerSecond(8);
        }
    } else if (newState == AnimationState::Swimming) {
        std::vector<int> fireFrameIDs{232, 233};
        std::vector<int> superFrameIDs{32, 33};
        std::vector<int> normalFrameIDs{7, 8};
        const int frameCount = 2;
        const int framesPerSecond = 16;
        if (!m_mario->hasComponent<AnimationComponent>()) {
            if (isFireMario()) {
                m_mario->addComponent<AnimationComponent>(fireFrameIDs,
                                                          framesPerSecond,
                                                          Map::PlayerIDCoordinates);
            } else if (isSuperStar()) {
                m_mario->addComponent<AnimationComponent>(superFrameIDs,
                                                          framesPerSecond,
                                                          Map::PlayerIDCoordinates);
            } else {
                m_mario->addComponent<AnimationComponent>(normalFrameIDs,
                                                          framesPerSecond,
                                                          Map::PlayerIDCoordinates);
                return;
            }
        }

        auto animation = m_mario->getComponent<AnimationComponent>();
        if (m_mario->getComponent<AnimationComponent>()->getFrameIDs() != superFrameIDs
            && m_mario->getComponent<AnimationComponent>()->getFrameIDs() != normalFrameIDs
            && m_mario->getComponent<AnimationComponent>()->getFrameIDs() != fireFrameIDs) {
            //If the player already has an animation but it is not the correct one

            animation->setFramesPerSecond(framesPerSecond);
            animation->setFrameCount(frameCount);
            //animation->setRepeated(true);
            if (isFireMario()) {
                animation->setFrameIDs(fireFrameIDs);
            } else if (isSuperMario()) {
                animation->setFrameIDs(superFrameIDs);
            } else {
                animation->setFrameIDs(normalFrameIDs);
            }
        }
    } else if (newState == AnimationState::Swimming_Jump) {
        std::vector<int> fireFrameIDs{232, 233, 234, 235, 236, 237};
        std::vector<int> superFrameIDs{32, 33, 34, 35, 36, 37};
        std::vector<int> normalFrameIDs{7, 8, 9, 10, 11, 11};
        const int frameCount = 6;
        const int framesPerSecond = 16;
        if (!m_mario->hasComponent<AnimationComponent>()) {
            if (isFireMario()) {
                m_mario->addComponent<AnimationComponent>(fireFrameIDs,
                                                          framesPerSecond,
                                                          Map::PlayerIDCoordinates,
                                                          false);
            } else if (isSuperStar()) {
                m_mario->addComponent<AnimationComponent>(superFrameIDs,
                                                          framesPerSecond,
                                                          Map::PlayerIDCoordinates,
                                                          false);
            } else {
                m_mario->addComponent<AnimationComponent>(normalFrameIDs,
                                                          framesPerSecond,
                                                          Map::PlayerIDCoordinates,
                                                          false);
                return;
            }
        }

        auto animation = m_mario->getComponent<AnimationComponent>();
        if (m_mario->getComponent<AnimationComponent>()->getFrameIDs() != superFrameIDs
            && m_mario->getComponent<AnimationComponent>()->getFrameIDs() != normalFrameIDs
            && m_mario->getComponent<AnimationComponent>()->getFrameIDs() != fireFrameIDs) {
            //If the player already has an animation but it is not the correct one

            animation->setFramesPerSecond(framesPerSecond);
            animation->setFrameCount(frameCount);
            animation->setRepeated(false);
            if (isFireMario()) {
                animation->setFrameIDs(fireFrameIDs);
            } else if (isSuperMario()) {
                animation->setFrameIDs(superFrameIDs);
            } else {
                animation->setFrameIDs(normalFrameIDs);
            }
        }
    } else if (newState == AnimationState::Swimming_Walk) {
        std::vector<int> fireFrameIDs{227, 228, 229};
        std::vector<int> superFrameIDs{27, 28, 29};
        std::vector<int> normalFrameIDs{2, 3, 4};
        const int frameCount = 3;
        const int framesPerSecond = 4;
        if (!m_mario->hasComponent<AnimationComponent>()) {
            if (isFireMario()) {
                m_mario->addComponent<AnimationComponent>(fireFrameIDs,
                                                          framesPerSecond,
                                                          Map::PlayerIDCoordinates,
                                                          false);
            } else if (isSuperStar()) {
                m_mario->addComponent<AnimationComponent>(superFrameIDs,
                                                          framesPerSecond,
                                                          Map::PlayerIDCoordinates,
                                                          false);
            } else {
                m_mario->addComponent<AnimationComponent>(normalFrameIDs,
                                                          framesPerSecond,
                                                          Map::PlayerIDCoordinates,
                                                          false);
                return;
            }
        }

        auto animation = m_mario->getComponent<AnimationComponent>();
        if (m_mario->getComponent<AnimationComponent>()->getFrameIDs() != superFrameIDs
            && m_mario->getComponent<AnimationComponent>()->getFrameIDs() != normalFrameIDs
            && m_mario->getComponent<AnimationComponent>()->getFrameIDs() != fireFrameIDs) {
            //If the player already has an animation but it is not the correct one

            animation->setFramesPerSecond(framesPerSecond);
            animation->setFrameCount(frameCount);
            if (isFireMario()) {
                animation->setFrameIDs(fireFrameIDs);
            } else if (isSuperMario()) {
                animation->setFrameIDs(superFrameIDs);
            } else {
                animation->setFrameIDs(normalFrameIDs);
            }
        }
    } else if (newState == AnimationState::Drifting) {
        if (m_mario->hasComponent<AnimationComponent>()) {
            m_mario->remove<AnimationComponent>();
        }

        if (isFireMario()) {
            spritesheet->setSpritesheetCoordinates(Map::PlayerIDCoordinates.at(230));
        } else if (isSuperMario()) {
            spritesheet->setSpritesheetCoordinates(Map::PlayerIDCoordinates.at(30));
        } else {
            spritesheet->setSpritesheetCoordinates(Map::PlayerIDCoordinates.at(5));
        }
    } else if (newState == AnimationState::Jumping) {
        if (m_mario->hasComponent<AnimationComponent>()) {
            m_mario->remove<AnimationComponent>();
        }

        if (isFireMario()) {
            spritesheet->setSpritesheetCoordinates(Map::PlayerIDCoordinates.at(231));
        } else if (isSuperMario()) {
            spritesheet->setSpritesheetCoordinates(Map::PlayerIDCoordinates.at(31));
        } else {
            spritesheet->setSpritesheetCoordinates(Map::PlayerIDCoordinates.at(6));
        }
    } else if (newState == AnimationState::Ducking) {
        if (m_mario->hasComponent<AnimationComponent>()) {
            m_mario->remove<AnimationComponent>();
        }

        if (isFireMario()) {
            spritesheet->setSpritesheetCoordinates(Map::PlayerIDCoordinates.at(226));
        } else if (isSuperMario()) {
            spritesheet->setSpritesheetCoordinates(Map::PlayerIDCoordinates.at(26));
        }
    } else if (newState == AnimationState::Launch_Fireball) {
        if (m_mario->hasComponent<AnimationComponent>()) {
            m_mario->remove<AnimationComponent>();
        }

        if (m_mario->hasComponent<BottomCollisionComponent>()) {
            spritesheet->setSpritesheetCoordinates(Map::PlayerIDCoordinates.at(240));
        } else {
            spritesheet->setSpritesheetCoordinates(Map::PlayerIDCoordinates.at(243));
        }
    } else if (newState == AnimationState::Climbing) {
        std::vector<int> fireFrameIDs{238, 239};
        std::vector<int> superFrameIDs{38, 39};
        std::vector<int> normalFrameIDs{13, 14};
        const int frameCount = 2;
        const int framesPerSecond = 8;
        if (!m_mario->hasComponent<AnimationComponent>()) {
            if (isFireMario()) {
                m_mario->addComponent<AnimationComponent>(fireFrameIDs,
                                                          framesPerSecond,
                                                          Map::PlayerIDCoordinates);
            } else if (isSuperMario()) {
                m_mario->addComponent<AnimationComponent>(superFrameIDs,
                                                          framesPerSecond,
                                                          Map::PlayerIDCoordinates);
            } else {
                m_mario->addComponent<AnimationComponent>(normalFrameIDs,
                                                          framesPerSecond,
                                                          Map::PlayerIDCoordinates);
                return;
            }
        }

        auto animation = m_mario->getComponent<AnimationComponent>();
        if (m_mario->getComponent<AnimationComponent>()->getFrameIDs() != superFrameIDs
            && m_mario->getComponent<AnimationComponent>()->getFrameIDs() != normalFrameIDs
            && m_mario->getComponent<AnimationComponent>()->getFrameIDs() != fireFrameIDs) {
            //If the player already has an animation but it is not the correct one

            animation->setFramesPerSecond(framesPerSecond);
            animation->setFrameCount(frameCount);
            if (isFireMario()) {
                animation->setFrameIDs(fireFrameIDs);
            } else if (isSuperMario()) {
                animation->setFrameIDs(superFrameIDs);
            } else {
                animation->setFrameIDs(normalFrameIDs);
            }
        }
    } else if (newState == AnimationState::Sliding) {
        if (m_mario->hasComponent<AnimationComponent>()) {
            m_mario->remove<AnimationComponent>();
        }

        if (isFireMario()) {
            spritesheet->setSpritesheetCoordinates(Map::PlayerIDCoordinates.at(238));
        } else if (isSuperMario()) {
            spritesheet->setSpritesheetCoordinates(Map::PlayerIDCoordinates.at(38));
        } else {
            spritesheet->setSpritesheetCoordinates(Map::PlayerIDCoordinates.at(13));
        }
    } else if (newState == AnimationState::Gameover) {
        if (m_mario->hasComponent<AnimationComponent>()) {
            m_mario->remove<AnimationComponent>();
        }

        spritesheet->setSpritesheetCoordinates(Map::PlayerIDCoordinates.at(1));
    }

    auto transform = m_mario->getComponent<TransformComponent>();

    if (isSuperMario() || isFireMario()) {
        if (newState == AnimationState::Ducking) {
            transform->getHitbox().size.y = 32.0f;
            transform->getHitbox().position.y = 32.0f;
        } else {
            transform->getHitbox().size.y = 64.0f;
            transform->getHitbox().position.y = 0.0f;
        }
    }

    m_currentState = newState;
}

void PlayerSystem::setPlayerState(PlayerState newState)
{
    if (newState == PlayerState::SuperMario || newState == PlayerState::FireMario) {
        std::vector<int> frameIDs;
        if (newState == PlayerState::SuperMario) {
            frameIDs = {46, 45, 25, 46, 45, 25, 46, 45, 25};
        } else {
            frameIDs = {350, 351, 352, 353, 350, 351, 352, 353, 350, 351, 352, 353};
        }
        m_mario->remove<AnimationComponent>();
        auto transform = m_mario->getComponent<TransformComponent>();
        auto spritesheet = m_mario->getComponent<SpritesheetComponent>();
        transform->setTop(transform->getTop() - GLOBALS::SCALED_CUBE_SIZE);
        transform->getHitbox().size.y = 2 * GLOBALS::SCALED_CUBE_SIZE;
        spritesheet->setEntityHeight(2.0f * GLOBALS::ORIGINAL_CUBE_SIZE);

        const int framesPerSecond = 12;
        m_mario->addComponent<AnimationComponent>(frameIDs,
                                                  framesPerSecond,
                                                  Map::PlayerIDCoordinates,
                                                  false);
        m_mario->getComponent<PlayerComponent>()->setPlayerState(PlayerState::SuperMario);
    }
}

void PlayerSystem::setUnderwater(bool on)
{
    m_underwater = on;
}

void PlayerSystem::updateCamera()
{
    auto transform = m_mario->getComponent<TransformComponent>();
    auto move = m_mario->getComponent<MovingComponent>();

    if (!Camera::GetInstance().isFrozen()) {
        if (transform->getPosition().x + 16 > Camera::GetInstance().getCameraCenterX()
            && move->getVelocity().x > 0.0f) {
            Camera::GetInstance().increaseCameraX(move->getVelocity().x);
        }

        if (transform->getPosition().x <= Camera::GetInstance().getCameraLeft()) {
            transform->setLeft(Camera::GetInstance().getCameraLeft());
        }

        if (transform->getRight() >= Camera::GetInstance().getCameraMaxX()) {
            transform->setRight(Camera::GetInstance().getCameraMaxX());
        }

        if (Camera::GetInstance().getCameraRight() >= Camera::GetInstance().getCameraMaxX()) {
            Camera::GetInstance().setCameraRight(Camera::GetInstance().getCameraMaxX());
        }

        Camera::GetInstance().updateCameraMin();
    }
}

void PlayerSystem::checkGameTime()
{
    if (m_world->hasSystem<HUDSystem>()) {
        if (m_world->getSystem<HUDSystem>()->getGameTime() < 0) {
            onGameOver(true);
        }
    }
}

void PlayerSystem::checkCollisionWithBreakBlock()
{
    m_world->find<BumpableComponent, TransformComponent, BottomCollisionComponent>(
        [=, this](Entity *breakable) {
            auto breakableTransform = breakable->getComponent<TransformComponent>();
            auto marioMove = m_mario->getComponent<MovingComponent>();
            auto marioTransform = m_mario->getComponent<TransformComponent>();
            int x1 = marioTransform->getPosition().x;
            int y1 = marioTransform->getPosition().y;
            int w1 = marioTransform->getHitbox().position.x + marioTransform->getHitbox().size.x;
            int h1 = marioTransform->getHitbox().position.y + marioTransform->getHitbox().size.y;

            int x2 = breakableTransform->getPosition().x;
            int y2 = breakableTransform->getPosition().y;
            int w2 = breakableTransform->getHitbox().position.x
                     + breakableTransform->getHitbox().size.x;
            int h2 = breakableTransform->getHitbox().position.y
                     + breakableTransform->getHitbox().size.y;
            bool isMarioCollidedWithBlock = AABB::isCollision(x1, y1, w1, h1, x2, y2, w2, h2);
            if (marioMove->getVelocity().y > 0.0f) {
                return;
            }

            // Destroy the block if the player is Super Mario
            if (!isSmallMario()) {
                if (!breakable->hasComponent<MysteryBoxComponent>()
                    && breakable->hasComponent<DestructibleComponent>()
                    && isMarioCollidedWithBlock) {
                    // This allows the enemy system to that the enemy should be destroyed, otherwise
                    // the enemy will fall as normal
                    std::vector<int> yChanges{0};
                    breakable->addComponent<BlockBumpComponent>(yChanges);
                    breakable->addComponent<CallbackComponent>(
                        [=](Entity *breakable) {
                            createBlockDebris(breakable);
                            m_world->destroy(breakable);

                            Entity *breakSound(m_world->create());
                            breakSound->addComponent<SoundComponent>(SoundID::BlockBreak);
                        },
                        1);
                    return;
                }
            }
            //If the player is in normal state, make the block bump
            if (!breakable->hasComponent<BlockBumpComponent>() && isMarioCollidedWithBlock) {
                std::vector<int> yChanges{-3, -3, -2, -1, 1, 2, 3, 3};
                breakable->addComponent<BlockBumpComponent>(yChanges);

                Entity *bumpSound(m_world->create());
                bumpSound->addComponent<SoundComponent>(SoundID::BlockHit);
            }

            breakable->remove<BottomCollisionComponent>();
            if (breakable->hasComponent<MysteryBoxComponent>()
                && !breakable->hasComponent<InvisibleBlockComponent>()
                && isMarioCollidedWithBlock) {
                auto mysteryBox = breakable->getComponent<MysteryBoxComponent>();
                breakable->getComponent<MysteryBoxComponent>()->whenDispensed()(breakable);
                breakable->remove<AnimationComponent>();
                breakable->getComponent<SpritesheetComponent>()->setSpritesheetCoordinates(
                    mysteryBox->deactivatedCoordinates());
                breakable->remove<BumpableComponent>();
            } else if (breakable->hasComponent<MysteryBoxComponent>()
                       && breakable->hasComponent<InvisibleBlockComponent>()) {
                auto mysteryBox = breakable->getComponent<MysteryBoxComponent>();

                breakable->remove<InvisibleBlockComponent>();
                marioMove->getVelocity().y = 0.0f;
                marioMove->getAcceleration().y = 0.0f;

                breakable->getComponent<MysteryBoxComponent>()->whenDispensed()(breakable);
                breakable->remove<AnimationComponent>();
                breakable->getComponent<SpritesheetComponent>()->setSpritesheetCoordinates(
                    mysteryBox->deactivatedCoordinates());
                breakable->remove<BumpableComponent>();
            }
        });
}

void PlayerSystem::checkCollisionWithPowerUp()
{
    m_world->find<CollectibleComponent, TransformComponent>([this](Entity *collectible) {
        if (!Camera::GetInstance().inCameraRange(collectible->getComponent<TransformComponent>())) {
            return;
        }
        if (!AABB::isCollision(collectible->getComponent<TransformComponent>(),
                               m_mario->getComponent<TransformComponent>())) {
            return;
        }

        auto collect = collectible->getComponent<CollectibleComponent>();
        switch (collect->type()) {
        case CollectibleType::Mushroom: {
            grow(GrowType::Mushroom);
            m_world->destroy(collectible);
        } break;
        case CollectibleType::SuperStar: {
            grow(GrowType::SuperStart);
            m_world->destroy(collectible);
        } break;
        case CollectibleType::FireFlower: {
            grow(GrowType::FireFlower);
            m_world->destroy(collectible);
        } break;
        case CollectibleType::Coin: {
            Entity *coinScore(m_world->create());
            coinScore->addComponent<AddScoreComponent>(100, true);

            Entity *coinSound(m_world->create());
            coinSound->addComponent<SoundComponent>(SoundID::Coin);

            m_world->destroy(collectible);
        } break;
        case CollectibleType::OneUp: {
            grow(GrowType::OneUp);

            Entity *oneUpSound(m_world->create());
            oneUpSound->addComponent<SoundComponent>(SoundID::OneUp);

            m_world->destroy(collectible);
        } break;
        }
    });
}

void PlayerSystem::checkCollisionWithProjectile()
{
    m_world->find<ProjectileComponent, TransformComponent>([this](Entity *projectile) {
        auto *projectileTransform = projectile->getComponent<TransformComponent>();
        if (!Camera::GetInstance().inCameraRange(projectileTransform)) {
            return;
        }

        if (!AABB::isTotalCollision(m_mario->getComponent<TransformComponent>(), projectileTransform)
            || isSuperStar()
            || m_mario->hasAny<EndingBlinkComponent, FrozenComponent, ParticleComponent>()) {
            return;
        }

        if (projectile->getComponent<ProjectileComponent>()->type() != ProjectileType::Fireball) {
            onGameOver(false);
        }
    });
}

void PlayerSystem::checkCollisionWithTrampoline()
{
    m_world->find<TrampolineComponent>([this](Entity *entity) {
        auto transform = m_mario->getComponent<TransformComponent>();
        auto move = m_mario->getComponent<MovingComponent>();
        auto *trampoline = entity->getComponent<TrampolineComponent>();
        auto *trampolineTransform = entity->getComponent<TransformComponent>();
        auto *trampolineTexture = entity->getComponent<SpritesheetComponent>();

        Entity *bottomEntity = trampoline->getBottomEntity();
        auto bottomTexture = bottomEntity->getComponent<SpritesheetComponent>();

        int x1 = transform->getPosition().x;
        int y1 = transform->getPosition().y;
        int w1 = transform->getHitbox().position.x + transform->getHitbox().size.x;
        int h1 = transform->getHitbox().position.y + transform->getHitbox().size.y;

        int x2 = trampolineTransform->getPosition().x;
        int y2 = trampolineTransform->getPosition().y;
        int w2 = trampolineTransform->getHitbox().position.x
                 + trampolineTransform->getHitbox().size.x;
        int h2 = trampolineTransform->getHitbox().position.y
                 + trampolineTransform->getHitbox().size.y;
        if (!AABB::isCollision(x1, y1, w1, h1, x2, y2, w2, h2)
            || !Camera::GetInstance().inCameraRange(trampolineTransform)) {
            trampoline->getCurrentSequenceIndex() = 0;
            return;
        }

        if (trampoline->getCurrentSequenceIndex() > 20) {
            return;
        }

        entity->remove<TileComponent>();

        switch (trampoline->getCurrentSequenceIndex()) {
        case 0: // Currently extended, set to half retracted
        {
            trampolineTexture->setSpritesheetCoordinates(
                Map::BlockIDCoordinates.at(trampoline->topMediumRetractedID()));
            bottomTexture->setSpritesheetCoordinates(
                Map::BlockIDCoordinates.at(trampoline->bottomMediumRetractedID()));
            trampolineTransform->getHitbox() = sf::Rect<int>({0, 16},
                                                             {GLOBALS::SCALED_CUBE_SIZE, 16});
        } break;
        case 1: // Currently half retracted, set to retracted
        {
            trampolineTexture->setSpritesheetCoordinates(
                Map::BlockIDCoordinates.at(trampoline->topRetractedID()));
            bottomTexture->setSpritesheetCoordinates(
                Map::BlockIDCoordinates.at(trampoline->bottomRetractedID()));
            trampolineTransform->getHitbox() = sf::Rect<int>({0, 32},
                                                             {GLOBALS::SCALED_CUBE_SIZE, 0});
        } break;

        case 2: // Currently retracted, set to half retracted and launch the player
        {
            trampolineTexture->setSpritesheetCoordinates(
                Map::BlockIDCoordinates.at(trampoline->topMediumRetractedID()));
            bottomTexture->setSpritesheetCoordinates(
                Map::BlockIDCoordinates.at(trampoline->bottomMediumRetractedID()));
            trampolineTransform->getHitbox() = sf::Rect<int>({0, 16},
                                                             {GLOBALS::SCALED_CUBE_SIZE, 16});
            move->getVelocity().y = -11.0f;
        } break;

        case 3: // Currently retracted, set to half retracted and launch the player
        {
            trampolineTexture->setSpritesheetCoordinates(
                Map::BlockIDCoordinates.at(trampoline->topExtendedID()));
            bottomTexture->setSpritesheetCoordinates(
                Map::BlockIDCoordinates.at(trampoline->bottomExtended()));
            trampolineTransform->getHitbox() = sf::Rect<int>({0, 0},
                                                             {GLOBALS::SCALED_CUBE_SIZE,
                                                              GLOBALS::SCALED_CUBE_SIZE});
            move->getVelocity().y = -11.0f;
        } break;
        }

        trampoline->getCurrentSequenceIndex()++;
        if (transform->getCenterY() > trampolineTransform->getBottom()) {
            transform->setCenterY(trampolineTransform->getBottom());
        }
    });
}

void PlayerSystem::checkCollisionWithMovingPlatform()
{
    int platformMoved = false;
    m_world->find<MovingPlatformComponent, MovingComponent>([&, this](Entity *entity) {
        auto transform = m_mario->getComponent<TransformComponent>();
        int x1 = m_mario->getComponent<TransformComponent>()->getPosition().x;
        int y1 = m_mario->getComponent<TransformComponent>()->getPosition().y;
        int w1 = m_mario->getComponent<TransformComponent>()->getHitbox().position.x
                 + m_mario->getComponent<TransformComponent>()->getHitbox().size.x;
        int h1 = m_mario->getComponent<TransformComponent>()->getHitbox().position.y
                 + m_mario->getComponent<TransformComponent>()->getHitbox().size.y;

        int x2 = entity->getComponent<TransformComponent>()->getPosition().x;
        int y2 = entity->getComponent<TransformComponent>()->getPosition().y;
        int w2 = entity->getComponent<TransformComponent>()->getHitbox().position.x
                 + entity->getComponent<TransformComponent>()->getHitbox().size.x;
        int h2 = entity->getComponent<TransformComponent>()->getHitbox().position.y
                 + entity->getComponent<TransformComponent>()->getHitbox().size.y;
        if (!AABB::isCollision(x1, y1, w1, h1, x2, y2, w2, h2) || platformMoved) {
            return;
        }

        auto platformMove = entity->getComponent<MovingComponent>();
        transform->setLeft(transform->getPosition().x + platformMove->getVelocity().x);
        transform->setBottom(entity->getComponent<TransformComponent>()->getTop()
                             + platformMove->getVelocity().y);

        if (transform->getPosition().x + 16 > Camera::GetInstance().getCameraCenterX()
            && platformMove->getVelocity().x > 0.0f) {
            Camera::GetInstance().increaseCameraX(platformMove->getVelocity().x);
        }

        platformMoved = true;
    });
}

void PlayerSystem::checkEnemyCollisions()
{
    bool enemyCrushed = false;
    auto *transform = m_mario->getComponent<TransformComponent>();
    auto *move = m_mario->getComponent<MovingComponent>();

    m_world->find<EnemyComponent, TransformComponent>([&](Entity *enemy) {
        if (!AABB::isTotalCollision(enemy->getComponent<TransformComponent>(), transform)
            || m_mario->hasComponent<FrozenComponent>() || enemy->hasComponent<DeadComponent>()
            || m_currentState == AnimationState::Gameover) {
            return;
        }

        auto *enemyMove = enemy->getComponent<MovingComponent>();
        auto *enemyPosition = enemy->getComponent<TransformComponent>();

        switch (enemy->getComponent<EnemyComponent>()->type()) {
        case EnemyType::KoopaShell: {
            if (isSuperStar()) {
                enemy->getComponent<MovingComponent>()->getVelocity().x = 0.0f;
                enemy->addComponent<EnemyDestroyedComponent>();

                // Entity* score(world->create());
                // score->addComponent<AddScoreComponent>(100);
                break;
            }

            if (move->getVelocity().y > 0.0f) {
                if (enemyMove->getVelocity().x != 0.0f) {
                    enemyMove->getVelocity().x = 0.0f;
                    move->getVelocity().y = -GLOBALS::ENEMY_SPEED;

                    enemyCrushed = true;
                } else {
                    enemyMove->getVelocity().x = 6.0f;
                }
            } else if (transform->getLeft() <= enemyPosition->getLeft()
                       && transform->getRight() < enemyPosition->getRight()
                       && move->getVelocity().y <= 0.0f) { // Hit from left side
                enemyMove->getVelocity().x = 6.0f;
            } else if (transform->getLeft() > enemyPosition->getLeft()
                       && transform->getRight() > enemyPosition->getRight()) {
                enemyMove->getVelocity().x = -6.0f;
            }
        } break;
        case EnemyType::FireBar: {
            if (!isSuperStar() && !m_mario->hasComponent<EndingBlinkComponent>()) {
                onGameOver(false);
            }
        } break;
        case EnemyType::KoopaParatroopa: {
            if (isSuperStar()) {
                enemy->getComponent<MovingComponent>()->getVelocity().x = 0.0f;

                enemy->addComponent<EnemyDestroyedComponent>();

                Entity *score(m_world->create());
                score->addComponent<AddScoreComponent>(100);
                return;
            }
            if (move->getVelocity().y > 0.0f && enemy->hasComponent<CrushableComponent>()) {
                enemy->addComponent<CrushedComponent>();
                transform->setBottom(enemyPosition->getTop());
                move->getVelocity().y = -GLOBALS::MARIO_BOUNCE;

                enemyCrushed = true;

                Entity *score(m_world->create());
                score->addComponent<AddScoreComponent>(100);

            } else if (!enemyCrushed && move->getVelocity().y <= 0.0f
                       && !m_mario->hasAny<FrozenComponent, EndingBlinkComponent>()) {
                onGameOver(false);
            } else if (enemyCrushed) {
                enemy->addComponent<CrushedComponent>();
                move->getVelocity().y = -GLOBALS::MARIO_BOUNCE;

                Entity *score(m_world->create());
                score->addComponent<AddScoreComponent>(100);
            }

        } break;
        case EnemyType::CheepCheep: {
            if (isSuperStar()) {
                enemy->getComponent<MovingComponent>()->getVelocity().x = 0.0f;

                enemy->addComponent<EnemyDestroyedComponent>();

                Entity *score(m_world->create());
                score->addComponent<AddScoreComponent>(100);
                return;
            }
            if ((move->getVelocity().y > 0.0f
                 || (move->getVelocity().y == 0.0f && enemyMove->getVelocity().y < 0.0f))
                && enemy->hasComponent<CrushableComponent>()) {
                enemy->addComponent<CrushedComponent>();
                enemy->getComponent<MovingComponent>()->getVelocity().x = 0.0f;
                move->getVelocity().y = -GLOBALS::MARIO_BOUNCE;

                enemyCrushed = true;

                Entity *score(m_world->create());
                score->addComponent<AddScoreComponent>(100);

            } else if (!enemyCrushed && move->getVelocity().y <= 0.0f
                       && !m_mario->hasAny<FrozenComponent, EndingBlinkComponent>()) {
                onGameOver(false);
            } else if (enemyCrushed) {
                enemy->addComponent<CrushedComponent>();
                enemy->getComponent<MovingComponent>()->getVelocity().x = 0.0f;
                move->getVelocity().y = -GLOBALS::MARIO_BOUNCE;

                Entity *score(m_world->create());
                score->addComponent<AddScoreComponent>(100);
            }
        } break;
        default: {
            if (isSuperStar()) {
                enemy->getComponent<MovingComponent>()->getVelocity().x = 0.0f;

                enemy->addComponent<EnemyDestroyedComponent>();

                Entity *score(m_world->create());
                score->addComponent<AddScoreComponent>(100);
                return;
            }
            if (move->getVelocity().y > 0.0f && enemy->hasComponent<CrushableComponent>()) {
                enemy->addComponent<CrushedComponent>();
                enemy->getComponent<MovingComponent>()->getVelocity().x = 0.0f;
                move->getVelocity().y = -GLOBALS::MARIO_BOUNCE;

                enemyCrushed = true;

                Entity *score(m_world->create());
                score->addComponent<AddScoreComponent>(100);

            } else if (!enemyCrushed && move->getVelocity().y <= 0.0f
                       && !m_mario->hasAny<FrozenComponent, EndingBlinkComponent>()) {
                onGameOver(false);
            } else if (enemyCrushed) {
                enemy->addComponent<CrushedComponent>();
                enemy->getComponent<MovingComponent>()->getVelocity().x = 0.0f;
                move->getVelocity().y = -GLOBALS::MARIO_BOUNCE;

                Entity *score(m_world->create());
                score->addComponent<AddScoreComponent>(100);
            }
        } break;
        }
    });
}

void PlayerSystem::onGameOver(bool outOfBounds)
{
    auto transform = m_mario->getComponent<TransformComponent>();
    auto spritesheet = m_mario->getComponent<SpritesheetComponent>();

    if (outOfBounds && (isSuperMario() || isFireMario())) {
        transform->setTop(transform->getTop() + GLOBALS::SCALED_CUBE_SIZE);
        transform->setHitbox(0, 0, GLOBALS::SCALED_CUBE_SIZE, GLOBALS::SCALED_CUBE_SIZE);

        spritesheet->setEntityHeight(GLOBALS::ORIGINAL_CUBE_SIZE);
        spritesheet->setSpritesheetCoordinates(Map::PlayerIDCoordinates.at(1));
        m_mario->getComponent<PlayerComponent>()->setPlayerState(PlayerState::SmallMario);
    }

    if (isSuperStar()) {
        m_mario->getComponent<PlayerComponent>()->setSuperStar(false);
    }

    if (m_underwater) {
        m_underwater = false;
    }

    if (!outOfBounds && (isSuperMario() || isFireMario())) {
        shrink();
        return;
    }

    auto move = m_mario->getComponent<MovingComponent>();
    move->getVelocity().x = 0.0f;
    move->getAcceleration().x = 0.0f;
    move->getVelocity().y = -12.5f;

    m_mario->addComponent<ParticleComponent>();
    spritesheet->setSpritesheetCoordinates(Map::PlayerIDCoordinates.at(1));
    m_currentState = AnimationState::Gameover;
    auto deathSound = m_world->create();
    deathSound->addComponent<SoundComponent>(SoundID::Death);

    m_mario->addComponent<CallbackComponent>(
        [](Entity *entity) {
            entity->remove<ParticleComponent>(); /*restartLevel*/
        },
        180);
}

void PlayerSystem::createBlockDebris(Entity *block)
{
    auto blockTransform = block->getComponent<TransformComponent>();

    Entity *topLeftDebris = m_world->create();
    topLeftDebris->addComponent<TransformComponent>(
        sf::Vector2f{blockTransform->getLeft(),
                     blockTransform->getTop() - GLOBALS::SCALED_CUBE_SIZE});
    topLeftDebris->addComponent<GravityComponent>();
    topLeftDebris->addComponent<MovingComponent>(sf::Vector2f{-8.0f, -2.0f},
                                                 sf::Vector2f{0.0f, 0.0f});
    topLeftDebris->addComponent<TextureComponent>(
        Game::s_textureHolder->get(TextureID::BlockTileSheet));
    topLeftDebris->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                                      GLOBALS::ORIGINAL_CUBE_SIZE,
                                                      1,
                                                      1,
                                                      1,
                                                      GLOBALS::ORIGINAL_CUBE_SIZE,
                                                      GLOBALS::ORIGINAL_CUBE_SIZE,
                                                      block->getComponent<DestructibleComponent>()
                                                          ->getDebrisCoordinates());
    topLeftDebris->addComponent<ParticleComponent>();
    topLeftDebris->addComponent<DestroyOutsideCameraComponent>();

    Entity *topRightDebris = m_world->create();
    topRightDebris->addComponent<TransformComponent>(
        sf::Vector2f{blockTransform->getLeft(),
                     blockTransform->getTop() - GLOBALS::SCALED_CUBE_SIZE});
    topRightDebris->addComponent<GravityComponent>();
    topRightDebris->addComponent<MovingComponent>(sf::Vector2f{8.0f, -2.0f},
                                                  sf::Vector2f{0.0f, 0.0f});
    topRightDebris->addComponent<TextureComponent>(
        Game::s_textureHolder->get(TextureID::BlockTileSheet));
    topRightDebris->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                                       GLOBALS::ORIGINAL_CUBE_SIZE,
                                                       1,
                                                       1,
                                                       1,
                                                       GLOBALS::ORIGINAL_CUBE_SIZE,
                                                       GLOBALS::ORIGINAL_CUBE_SIZE,
                                                       block->getComponent<DestructibleComponent>()
                                                           ->getDebrisCoordinates());
    topRightDebris->addComponent<ParticleComponent>();
    topRightDebris->addComponent<DestroyOutsideCameraComponent>();

    Entity *bottomLeftDebris = m_world->create();
    bottomLeftDebris->addComponent<TransformComponent>(
        sf::Vector2f{blockTransform->getLeft(), blockTransform->getTop()});
    bottomLeftDebris->addComponent<GravityComponent>();
    bottomLeftDebris->addComponent<MovingComponent>(sf::Vector2f{-8.0f, -2.0f},
                                                    sf::Vector2f{0.0f, 0.0f});
    bottomLeftDebris->addComponent<TextureComponent>(
        Game::s_textureHolder->get(TextureID::BlockTileSheet));
    bottomLeftDebris->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                                         GLOBALS::ORIGINAL_CUBE_SIZE,
                                                         1,
                                                         1,
                                                         1,
                                                         GLOBALS::ORIGINAL_CUBE_SIZE,
                                                         GLOBALS::ORIGINAL_CUBE_SIZE,
                                                         block
                                                             ->getComponent<DestructibleComponent>()
                                                             ->getDebrisCoordinates());
    bottomLeftDebris->addComponent<ParticleComponent>();
    bottomLeftDebris->addComponent<DestroyOutsideCameraComponent>();

    Entity *bottomRightDebris = m_world->create();
    bottomRightDebris->addComponent<TransformComponent>(
        sf::Vector2f{blockTransform->getLeft(), blockTransform->getTop()});
    bottomRightDebris->addComponent<GravityComponent>();
    bottomRightDebris->addComponent<MovingComponent>(sf::Vector2f{8.0f, -2.0f},
                                                     sf::Vector2f{0.0f, 0.0f});
    bottomRightDebris->addComponent<TextureComponent>(
        Game::s_textureHolder->get(TextureID::BlockTileSheet));
    bottomRightDebris->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                                          GLOBALS::ORIGINAL_CUBE_SIZE,
                                                          1,
                                                          1,
                                                          1,
                                                          GLOBALS::ORIGINAL_CUBE_SIZE,
                                                          GLOBALS::ORIGINAL_CUBE_SIZE,
                                                          block
                                                              ->getComponent<DestructibleComponent>()
                                                              ->getDebrisCoordinates());
    bottomRightDebris->addComponent<ParticleComponent>();
    bottomRightDebris->addComponent<DestroyOutsideCameraComponent>();
}

void PlayerSystem::createFireball()
{
    m_holdFire = true;

    m_currentState = AnimationState::Launch_Fireball;

    Entity *tempCallback(m_world->create());

    tempCallback->addComponent<CallbackComponent>(
        [this](Entity *entity) {
            m_holdFire = false;

            m_world->destroy(entity);
        },
        16);

    Entity *fireball(m_world->create());
    auto *fireballTransform = fireball->addComponent<TransformComponent>(
        sf::Vector2f{},
        sf::Vector2i{1, 1},
        sf::Rect<int>{{16, 16}, {GLOBALS::SCALED_CUBE_SIZE / 2, GLOBALS::SCALED_CUBE_SIZE / 2}});

    fireball->addComponent<TextureComponent>(
        Game::s_textureHolder->get(TextureID::PlayerSprteSheet));
    fireball->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE / 2,
                                                 GLOBALS::ORIGINAL_CUBE_SIZE / 2,
                                                 1,
                                                 9,
                                                 0,
                                                 GLOBALS::ORIGINAL_CUBE_SIZE,
                                                 GLOBALS::ORIGINAL_CUBE_SIZE,
                                                 Map::PlayerIDCoordinates.at(246));

    auto *fireballMove = fireball->addComponent<MovingComponent>(sf::Vector2f(0.0f, 5.0f),
                                                                 sf::Vector2f(0.0f, 0.0f));

    fireball->addComponent<FrictionExemptComponent>();

    fireball->addComponent<GravityComponent>();

    fireball->addComponent<DestroyOutsideCameraComponent>();

    auto *marioTexture = m_mario->getComponent<TextureComponent>();

    if (marioTexture->isHorizontalFlipped()) {
        fireballTransform->setLeft(m_mario->getComponent<TransformComponent>()->getRight() - 32.0f);
        fireballMove->getVelocity().x = -GLOBALS::PROJECTILE_SPEED;
    } else {
        fireballTransform->setLeft(m_mario->getComponent<TransformComponent>()->getRight());
        fireballMove->getVelocity().x = GLOBALS::PROJECTILE_SPEED;
    }

    fireballTransform->setTop(m_mario->getComponent<TransformComponent>()->getTop() + 4);

    fireball->addComponent<WaitUntilComponent>(
        [=](Entity *entity) {
            return entity->hasAny<LeftCollisionComponent, RightCollisionComponent>()
                   || !Camera::GetInstance().inCameraRange(
                       entity->getComponent<TransformComponent>());
        },
        [this](Entity *entity) {
            entity->remove<WaitUntilComponent>();
            if (entity->hasAny<LeftCollisionComponent, RightCollisionComponent>()) {
                entity->getComponent<SpritesheetComponent>()->setSpritesheetCoordinates(
                    Map::PlayerIDCoordinates.at(247));
                entity->addComponent<DestroyDelayedComponent>(4);
                entity->remove<MovingComponent, GravityComponent, FrictionExemptComponent>();

                Entity *fireballHitSound(m_world->create());
                fireballHitSound->addComponent<SoundComponent>(SoundID::BlockHit);
            } else {
                m_world->destroy(entity);
            }
        });

    fireball->addComponent<ProjectileComponent>(ProjectileType::Fireball);
}

void PlayerSystem::shrink()
{
    m_mario->getComponent<PlayerComponent>()->setPlayerState(PlayerState::SmallMario);
    Entity *shrinkSound = m_world->create();
    shrinkSound->addComponent<SoundComponent>(SoundID::Shrink);

    std::vector<int> frameIDs{25, 45, 46, 25, 45, 46, 25, 45, 46};
    const int framesPerSecond = 12;

    m_mario->addComponent<AnimationComponent>(frameIDs,
                                              framesPerSecond,
                                              Map::PlayerIDCoordinates,
                                              false);
    m_mario->addComponent<FrozenComponent>();
    m_mario->addComponent<CallbackComponent>(
        [](Entity *entity) {
            auto *transform = entity->getComponent<TransformComponent>();
            auto *spritesheet = entity->getComponent<SpritesheetComponent>();

            transform->setHitbox(0, 0, GLOBALS::SCALED_CUBE_SIZE, GLOBALS::SCALED_CUBE_SIZE);
            transform->move(sf::Vector2f{0.0f, GLOBALS::SCALED_CUBE_SIZE});
            transform->getSize().y = 1;
            spritesheet->setEntityHeight(GLOBALS::ORIGINAL_CUBE_SIZE);
            spritesheet->setSpritesheetCoordinates(Map::PlayerIDCoordinates.at(0));
            entity->remove<FrozenComponent>();
            entity->addComponent<EndingBlinkComponent>(10, 150);
        },
        45);
}

void PlayerSystem::grow(GrowType type)
{
    if (type == GrowType::OneUp) {
        auto addLives = m_world->create();
        addLives->addComponent<AddLivesComponent>(1);

        auto floatingText = m_world->create();
        floatingText->addComponent<CreateFloatingTextComponent>(m_mario, "1-UP");
    } else if (type == GrowType::SuperStart) {
        m_mario->getComponent<PlayerComponent>()->setSuperStar(true);
        m_mario->addComponent<EndingBlinkComponent>(1, 600);
        m_mario->addComponent<CallbackComponent>(
            [](Entity *entity) {
                auto player = entity->getComponent<PlayerComponent>();
                player->setSuperStar(false);
                //self.scene:resumeLastPlayedMusic()
            },
            600);
        /*
        local superStarMusic = Concord.entity(world)
        superStarMusic:give('music_component', MUSIC_ID.SUPER_STAR)
*/
    } else if (type == GrowType::Mushroom) {
        auto addScore = m_world->create();
        addScore->addComponent<AddScoreComponent>(1000);
        auto floatingText = m_world->create();
        floatingText->addComponent<CreateFloatingTextComponent>(m_mario, "1000");
        auto powerUpSound = m_world->create();
        powerUpSound->addComponent<SoundComponent>(SoundID::PowerUpCollect);
        if (isSuperMario() || isFireMario()) {
            return;
        }

        auto *transform = m_mario->getComponent<TransformComponent>();
        auto spritesheet = m_mario->getComponent<SpritesheetComponent>();

        transform->setTop(transform->getTop() - GLOBALS::SCALED_CUBE_SIZE);
        transform->setHitbox(0, 0, GLOBALS::SCALED_CUBE_SIZE, 2 * GLOBALS::SCALED_CUBE_SIZE);
        transform->getSize().y = 2;
        spritesheet->setEntityHeight(2 * GLOBALS::ORIGINAL_CUBE_SIZE);

        const std::vector<int> frameIDs = {46, 45, 25, 46, 45, 25, 46, 45, 25};
        const int framesPerSecond = 12;
        m_mario->addComponent<AnimationComponent>(frameIDs,
                                                  framesPerSecond,
                                                  Map::PlayerIDCoordinates,
                                                  false);
        m_mario->addComponent<FrozenComponent>();
        m_mario->addComponent<CallbackComponent>(
            [](Entity *entity) {
                auto player = entity->getComponent<PlayerComponent>();
                player->setPlayerState(PlayerState::SuperMario);
                entity->remove<FrozenComponent>();
            },
            45);
    } else if (type == GrowType::FireFlower) {
        if (!isSuperMario()) {
            grow(GrowType::Mushroom);
            return;
        }

        auto addScore = m_world->create();
        addScore->addComponent<AddScoreComponent>(1000);

        auto floatingText = m_world->create();
        floatingText->addComponent<CreateFloatingTextComponent>(m_mario, "1000");

        auto powerUpSound = m_world->create();
        powerUpSound->addComponent<SoundComponent>(SoundID::PowerUpCollect);

        if (isFireMario()) {
            return;
        }

        const std::vector<int> frameIDs{350, 351, 352, 353, 350, 351, 352, 353, 350, 351, 352, 353};
        const int framesPerSecond = 12;
        const bool repeated = false;
        m_mario->addComponent<AnimationComponent>(frameIDs,
                                                  framesPerSecond,
                                                  Map::PlayerIDCoordinates,
                                                  repeated);
        m_mario->addComponent<FrozenComponent>();
        m_mario->addComponent<CallbackComponent>(
            [](Entity *entity) {
                entity->getComponent<PlayerComponent>()->setPlayerState(PlayerState::FireMario);
                entity->remove<FrozenComponent>();
            },
            60);
    }
}

void PlayerSystem::launchFireballs()
{
    if (isFireMario() && m_launchFireball) {
        createFireball();
        m_launchFireball = 0;

        Entity *fireballSound(m_world->create());
        fireballSound->addComponent<SoundComponent>(SoundID::Fireball);
    }
}

void PlayerSystem::updateWaterVelocity()
{
    auto move = m_mario->getComponent<MovingComponent>();
    auto texture = m_mario->getComponent<TextureComponent>();

    if (!m_mario->hasComponent<FrictionExemptComponent>()) {
        m_mario->addComponent<FrictionExemptComponent>();
    }

    if (!m_mario->hasComponent<BottomCollisionComponent>()
        && !m_mario->hasComponent<WaitUntilComponent>()) {
        m_currentState = AnimationState::Swimming;
    } else if (m_mario->hasComponent<BottomCollisionComponent>()) {
        if (std::fabs(move->getVelocity().x) > 0.0f) {
            m_currentState = AnimationState::Swimming_Walk;
        } else {
            m_currentState = AnimationState::Standing;
        }
    }

    if (m_currentState == AnimationState::Swimming
        || m_currentState == AnimationState::Swimming_Jump) {
        if (m_left > 0) {
            move->getVelocity().x += m_underwaterControllerX.calculate(move->getVelocity().x, -3.0f);
            texture->setHorizontalFlipped(true);
        } else if (m_right > 0) {
            move->getVelocity().x += m_underwaterControllerX.calculate(move->getVelocity().x, 3.0f);
            texture->setHorizontalFlipped(false);
        } else {
            move->getVelocity().x += m_underwaterControllerX.calculate(move->getVelocity().x, 0.0f);
        }
    } else {
        if (m_left > 0) {
            move->getVelocity().x += m_underwaterControllerX.calculate(move->getVelocity().x, -1.0f);
            texture->setHorizontalFlipped(true);
        } else if (m_right > 0) {
            move->getVelocity().x += m_underwaterControllerX.calculate(move->getVelocity().x, 1.0f);
            texture->setHorizontalFlipped(false);
        } else {
            move->getVelocity().x += m_underwaterControllerX.calculate(move->getVelocity().x, 0.0f);
        }
    }
    move->getAcceleration().y = -0.5f;

    if (move->getVelocity().y > GLOBALS::MAX_UNDERWATER_Y) {
        move->getVelocity().y = GLOBALS::MAX_UNDERWATER_Y;
    }

    if (m_jump > 0 && m_jumpHeld > 0) {
        move->getVelocity().y = -3.5f;
        m_jumpHeld = 0;

        auto stompSound = m_world->create();
        stompSound->addComponent<SoundComponent>(SoundID::Stomp);

        m_currentState = AnimationState::Swimming_Jump;
        m_mario->addComponent<WaitUntilComponent>(
            [](Entity *entity) { return !entity->hasComponent<AnimationComponent>(); },
            [this](Entity *entity) {
                m_currentState = AnimationState::Swimming;
                entity->remove<WaitUntilComponent>();
            });
    }
    const int MIN_UNDERWATER_Y = 640;
    if (m_mario->getComponent<TransformComponent>()->getPosition().y <= MIN_UNDERWATER_Y) {
        move->getVelocity().y = 0.0f;
        m_mario->getComponent<TransformComponent>()->setTop(MIN_UNDERWATER_Y);
    }
}

void PlayerSystem::updateGroundVelocity()
{
    auto texture = m_mario->getComponent<TextureComponent>();
    auto move = m_mario->getComponent<MovingComponent>();

    if (m_left > 0) {
        texture->setHorizontalFlipped(true);
    } else if (m_right > 0) {
        texture->setHorizontalFlipped(false);
    }

    // Updates the acceleration
    move->getAcceleration().x = m_xDir * GLOBALS::MARIO_ACCELERATION_X;

    if (m_running > 0) {
        //a weird number that will max the velocity at 5
        move->getAcceleration().x *= 1.3297872340425531914f;
    } else {
        move->getAcceleration().x *= 0.7978723404255319148936f;
    }

    if (m_jump > 0 && m_jumpHeld > 0 && !m_trampolineCollided) {
        m_jumpHeld = 1;
        move->getVelocity().y = -7.3f;

        auto jumpSound = m_world->create();
        jumpSound->addComponent<SoundComponent>(SoundID::Jump);
    }

    if (m_duck > 0 && (isSuperMario() || isFireMario())) {
        m_currentState = AnimationState::Ducking;
        move->getAcceleration().x = 0.0f;
        //Slows the player down
        if (move->getVelocity().x > 1.5f) {
            move->getVelocity().x -= 0.5f;
        } else if (move->getVelocity().x < -1.5f) {
            move->getVelocity().x += 0.5f;
        }
    } else if (std::fabs(move->getVelocity().x) > 0.2f
               || std::fabs(move->getAcceleration().x > 0.2f)) {
        // If the player should be drifting
        if ((move->getVelocity().x > 0.0f && move->getAcceleration().x < 0.0f)
            && (move->getVelocity().x < 0.0f && move->getAcceleration().x > 0.0f)) {
            m_currentState = AnimationState::Drifting;
        } else {
            m_currentState = AnimationState::Walking;
        }
    } else {
        m_currentState = AnimationState::Standing;
    }
}

void PlayerSystem::updateAirVelocity()
{
    auto move = m_mario->getComponent<MovingComponent>();
    move->getAcceleration().x = m_xDir * GLOBALS::MARIO_ACCELERATION_X;

    if (m_running > 0) {
        if ((move->getAcceleration().x > 0.0f && move->getVelocity().x > 0.0f)
            || (move->getAcceleration().x < 0.0f && move->getVelocity().x < 0.0f)) {
            move->getAcceleration().x *= 1.5957446808510638297f;
        }
    }

    // Changes mario's acceleration while in the air (the longer you jump the higher mario will go
    if (m_jumpHeld > 0 && move->getVelocity().y < -1.0f) {
        if (m_running > 0 && fabs(move->getVelocity().x) > 3.5f) {
            move->getAcceleration().y = -0.414f;
        } else {
            move->getAcceleration().y = -0.412f;
        }
    } else {
        move->getAcceleration().y = 0.0f;
    }

    if (m_duck > 0 && (isSuperMario() || isFireMario())) {
        m_currentState = AnimationState::Ducking;
    } else {
        m_currentState = AnimationState::Jumping;
    }
}
