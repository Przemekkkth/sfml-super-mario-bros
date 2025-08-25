#ifndef WARPSYSTEM_H
#define WARPSYSTEM_H
#include "../../utils/ResourceIdentifiers.h"
#include "System.h"
#include <SFML/Graphics/Color.hpp>

class Entity;
class GameWorld;
class World;
class WarpSystem : public System
{
public:
    WarpSystem(GameWorld *gameWorld);

    virtual void onAddedToWorld(World *world) override;

    virtual void tick(World *world) override;

    virtual void handleInput(const std::optional<sf::Event> &event) override;

    void warp(Entity *pipe, Entity *player);

    void climb(Entity *vine, Entity *player);

    static bool isWarping() { return warping; }

    static bool isClimbing() { return climbing; }

    static bool hasClimbed() { return climbed; }

    static void setWarping(bool val) { warping = val; }

    static void setClimbing(bool val) { climbing = val; }

    static void setClimbed(bool val) { climbed = val; }

    void setTeleportLevelY(int levelY) { m_teleportLevelY = levelY; }

    void setTeleportPlayerCoordinates(sf::Vector2i playerCoordinates)
    {
        m_teleportPlayerCoordinates = playerCoordinates;
    }

    void setTeleportCameraCoordinates(sf::Vector2i cameraCoordinates)
    {
        m_teleportCameraCoordinates = cameraCoordinates;
    }

    void setTeleportCameraMax(int cameraMax) { m_teleportCameraMax = cameraMax; }

    void setTeleportBackgroundColor(sf::Color backgroundColor)
    {
        m_teleportBackgroundColor = backgroundColor;
    }

    void setTeleportLevelType(LevelType levelType) { m_teleportLevelType = levelType; }

    int m_up = 0;
    int m_down = 0;
    int m_left = 0;
    int m_right = 0;

private:
    static bool warping;
    static bool climbing;
    static bool climbed;

    GameWorld *m_scene;

    int m_teleportLevelY;
    sf::Vector2i m_teleportPlayerCoordinates;
    sf::Vector2i m_teleportCameraCoordinates;
    int m_teleportCameraMax;
    sf::Color m_teleportBackgroundColor;
    LevelType m_teleportLevelType;
    World *m_world;
};

#endif // WARPSYSTEM_H
