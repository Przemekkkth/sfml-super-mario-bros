#ifndef VINECOMPONENT_H
#define VINECOMPONENT_H
#include "../../LevelData.h"
#include "../../utils/ResourceIdentifiers.h"
#include "Component.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

class Entity;
class VineComponent : public Component
{
public:
    VineComponent(sf::Vector2i coordinates,
                  sf::Vector2i teleport,
                  sf::Vector2i camera,
                  int resetValue,
                  sf::Vector2i resetLocation,
                  int newCameraMax,
                  sf::Color newBackgroundColor,
                  LevelType newLevelType,
                  std::shared_ptr<std::vector<Entity *>> vineParts)
        : m_coordinates{coordinates}
        , m_teleportCoordinates{teleport}
        , m_cameraCoordinates{camera}
        , m_resetYValue{resetValue}
        , m_resetTeleportLocation{resetLocation}
        , m_newCameraMax{newCameraMax}
        , m_newBackgroundColor{newBackgroundColor}
        , m_newLevelType{newLevelType}
        , m_vineParts{std::move(vineParts)}
    {}

    VineComponent(LevelData::Vine &vineData, std::shared_ptr<std::vector<Entity *>> vineParts)
        : m_coordinates{vineData.vineCoords}
        , m_teleportCoordinates{vineData.teleportCoords}
        , m_cameraCoordinates{vineData.cameraCoords}
        , m_resetYValue{vineData.y}
        , m_resetTeleportLocation{vineData.resetLocation}
        , m_newCameraMax{vineData.cameraMax}
        , m_newBackgroundColor{vineData.bgColor}
        , m_newLevelType{vineData.levelType}
        , m_vineParts{std::move(vineParts)}
    {}

    sf::Vector2i &getCoordinates() { return m_coordinates; }

    sf::Vector2i &getTeleportCoordinates() { return m_teleportCoordinates; }

    sf::Vector2i &getCameraCoordinates() { return m_cameraCoordinates; }

    int &getResetYvalue() { return m_resetYValue; }

    sf::Vector2i getResetTeleportLocation() { return m_resetTeleportLocation; }

    int &getCameraMax() { return m_newCameraMax; }

    sf::Color &getNewBackgroundColor() { return m_newBackgroundColor; }

    LevelType &getNewLevelType() { return m_newLevelType; }

    std::shared_ptr<std::vector<Entity *>> getVineParts() { return m_vineParts; }

    void addPiece(Entity *entity) { m_vineParts->push_back(entity); }

    int size() { return m_vineParts->size(); }

private:
    sf::Vector2i m_coordinates;
    sf::Vector2i m_teleportCoordinates;
    sf::Vector2i m_cameraCoordinates;
    int m_resetYValue;
    sf::Vector2i m_resetTeleportLocation;
    int m_newCameraMax;
    sf::Color m_newBackgroundColor;
    LevelType m_newLevelType;
    std::shared_ptr<std::vector<Entity *>> m_vineParts;
};

#endif // VINECOMPONENT_H
