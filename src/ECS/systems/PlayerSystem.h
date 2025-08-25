#ifndef PLAYERSYSTEM_H
#define PLAYERSYSTEM_H
#include "../../LevelData.h"
#include "../../utils/PIDController.h"
#include "../Entity.h"
#include "System.h"

class LevelData;
class PlayerSystem : public System
{
public:
    PlayerSystem(LevelData levelData);

    virtual void onAddedToWorld(World *world) override;
    virtual void tick(World *world) override;
    virtual void handleInput(const std::optional<sf::Event> &event) override;
    virtual void handleInput() override;

    bool isSmallMario();
    bool isSuperMario();
    bool isFireMario();
    bool isSuperStar();
    bool isGameOver();
    void setCurrentState(AnimationState newState);
    void setPlayerState(PlayerState newState);
    void setUnderwater(bool on);
    static bool isInputEnabled();
    static void setEnableInput(bool on);

private:
    void updateCamera();
    void checkGameTime();
    void checkCollisionWithBreakBlock();
    void checkCollisionWithPowerUp();
    void checkCollisionWithProjectile();
    void checkCollisionWithTrampoline();
    void checkCollisionWithMovingPlatform();
    void checkEnemyCollisions();
    void onGameOver(bool outOfBounds);

    void createBlockDebris(Entity *block);
    void createFireball();

    void shrink();
    void grow(GrowType type);
    void launchFireballs();

    void updateWaterVelocity();
    void updateGroundVelocity();
    void updateAirVelocity();
    Entity *m_mario;

    int m_xDir = 0;
    int m_left = 0;
    int m_right = 0;
    int m_jump = 0;
    int m_duck = 0;
    int m_launchFireball = 0;
    int m_running = 0;

    int m_holdFireballTexture = 0;
    int m_jumpHeld = 0;
    bool m_underwater = false;
    bool m_trampolineCollided = false;
    bool m_holdFire = false;
    static bool m_enabled;
    LevelData m_levelData;
    AnimationState m_currentState;
    PlayerState m_playerState;
    World *m_world;
    PIDController m_underwaterControllerX;
};

#endif // PLAYERSYSTEM_H
