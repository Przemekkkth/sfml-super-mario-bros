#ifndef LEVELDATA_H
#define LEVELDATA_H
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <vector>
#include "utils/ResourceIdentifiers.h"

class LevelData
{
public:
    LevelData();


    struct Warp
    {
        sf::Vector2i pipeCoords;
        sf::Vector2i teleportCoords;
        sf::Vector2i cameraCoords;
        LevelType levelType;
        Direction goingIn;
        Direction goingOut;
        sf::Vector2i levelToGo;
        bool cameraFreeze;
        sf::Color bgColor;
    };

    struct MovingPlatform
    {
        sf::Vector2i coords;
        PlatformMotionType motion;
        Direction direction;
        sf::Vector2i point;
        bool shift;
    };

    struct FloatingText
    {
        sf::Vector2f pos;
        std::string text;
    };

    struct FireBar
    {
        sf::Vector2i coords;
        int angle;
        RotationDirection direction;
        int length;
    };

    struct Vine
    {
        sf::Vector2i vineCoords;
        sf::Vector2i teleportCoords;
        sf::Vector2i cameraCoords;
        int y;
        sf::Vector2i resetLocation;
        int cameraMax;
        sf::Color bgColor;
        LevelType levelType;
    };

    struct PlatformLevel
    {
        sf::Vector2i leftCoords;
        sf::Vector2i rightCoords;
        int yLevel;
    };

    sf::Vector2i getPlayerStart() const;
    LevelType getLevelType() const;
    sf::Vector2i getCameraStart() const;
    sf::Color getBackgroundColor() const;
    int getCameraMax() const;
    sf::Vector2i getNextLevel() const;
    std::vector<Warp> getWarpPipes() const;
    std::vector<MovingPlatform> getMovingPlatform() const;
    std::vector<FloatingText> getFloatingTexts() const;
    std::vector<FireBar> getFireBars() const;
    std::vector<Vine> getVines() const;
    std::vector<PlatformLevel> getPlatformLevels() const;

    void setBackgroundColor(sf::Color newColor);

private:
    void loadJSON();
    sf::Color convertStrToColor(std::string colorStr) const;
    Direction convertStrToDirection(std::string directionStr) const;
    LevelType convertStrToLevelType(std::string levelTypeStr) const;
    PlatformMotionType convertStrToPlatformMotionType(std::string platformMotionTypeStr) const;
    RotationDirection convertStrToRotationDirection(std::string rotationDirectionStr) const;

    sf::Vector2i m_playerStart;
    LevelType m_levelType;
    sf::Vector2i m_cameraStart;
    sf::Color m_backgroundColor;
    int m_cameraMax;
    sf::Vector2i m_nextLevel;
    std::vector<Warp> m_warps;
    std::vector<MovingPlatform> m_movingPlatforms;
    std::vector<FloatingText> m_floatingTexts;
    std::vector<FireBar> m_fireBars;
    std::vector<Vine> m_vines;
    std::vector<PlatformLevel> m_platformLevels;
};

#endif // LEVELDATA_H
