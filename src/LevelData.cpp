#include "LevelData.h"
#include "GameData.h"
#include "Globals.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <cassert>

using json = nlohmann::json;

LevelData::LevelData()
{
    loadJSON();
}

void LevelData::loadJSON()
{
    m_warps.clear();
    std::string pathToJSON = "assets/data/World" + std::to_string(GameData::LEVEL) + "-" + std::to_string(GameData::SUB_LEVEL) +
                             "/World" + std::to_string(GameData::LEVEL) + "-" + std::to_string(GameData::SUB_LEVEL) + "_properties.json";
    std::ifstream f(pathToJSON);

    json data = json::parse(f);
    m_playerStart.x = data["PLAYER_START"][0];
    m_playerStart.y = data["PLAYER_START"][1];
    m_backgroundColor = convertStrToColor(data["BACKGROUND_COLOR"]);
    m_cameraMax = data["CAMERA_MAX"];
    m_cameraStart.x = data["CAMERA_START"][0];
    m_cameraStart.y = data["CAMERA_START"][1];
    m_levelType = convertStrToLevelType(data["LEVEL_TYPE"]);
    m_nextLevel.x = data["NEXT_LEVEL"][0];
    m_nextLevel.y = data["NEXT_LEVEL"][1];

    if (data["WARP_PIPE"].is_array())
    {
        for(auto iter = data["WARP_PIPE"].begin(); iter != data["WARP_PIPE"].end(); ++iter)
        {
            Warp w;
            w.pipeCoords.x = (*iter)["pipe_coords"][0];
            w.pipeCoords.y = (*iter)["pipe_coords"][1];

            w.cameraCoords.x = (*iter)["camera_coords"][0];
            w.cameraCoords.y = (*iter)["camera_coords"][1];

            w.goingIn = convertStrToDirection((*iter)["going_in"]);
            w.goingOut = convertStrToDirection((*iter)["going_out"]);

            w.teleportCoords.x = (*iter)["teleport_coords"][0];
            w.teleportCoords.y = (*iter)["teleport_coords"][1];

            w.cameraFreeze = (*iter)["freeze_camera"];
            w.bgColor = convertStrToColor( (*iter)["bg_color"]);
            w.levelToGo.x = (*iter)["level_to_go"][0];
            w.levelToGo.y = (*iter)["level_to_go"][1];
            w.levelType = convertStrToLevelType((*iter)["level_type"]);
            m_warps.push_back(w);
        }
    }

    if (data["MOVING_PLATFORM"].is_array()) {
        for (auto iter = data["MOVING_PLATFORM"].begin(); iter != data["MOVING_PLATFORM"].end();
             ++iter) {
            MovingPlatform m;
            m.coords.x = (*iter)["coords"][0];
            m.coords.y = (*iter)["coords"][1];
            m.motion = convertStrToPlatformMotionType((*iter)["motion"]);
            m.direction = convertStrToDirection((*iter)["direction"]);
            m.point.x = (*iter)["point"][0];
            m.point.y = (*iter)["point"][1];
            m.shift = (*iter)["shift"];
            m_movingPlatforms.push_back(m);
        }
    }

    if (data["FLOATING_TEXT"].is_array()) {
        for (auto iter = data["FLOATING_TEXT"].begin(); iter != data["FLOATING_TEXT"].end();
             ++iter) {
            FloatingText t;
            t.pos.x = (*iter)["pos"][0];
            t.pos.y = (*iter)["pos"][1];
            t.text = (*iter)["text"];
            m_floatingTexts.push_back(t);
        }
    }

    if (data["FIRE_BAR"].is_array()) {
        for (auto iter = data["FIRE_BAR"].begin(); iter != data["FIRE_BAR"].end(); ++iter) {
            FireBar f;
            f.coords.x = (*iter)["coords"][0];
            f.coords.y = (*iter)["coords"][1];
            f.angle = (*iter)["angle"];
            f.direction = convertStrToRotationDirection((*iter)["direction"]);
            f.length = (*iter)["length"];
            m_fireBars.push_back(f);
        }
    }

    if (data["VINE"].is_array()) {
        for (auto iter = data["VINE"].begin(); iter != data["VINE"].end(); ++iter) {
            Vine v;
            v.vineCoords.x = (*iter)["vine_coords"][0];
            v.vineCoords.y = (*iter)["vine_coords"][1];
            v.teleportCoords.x = (*iter)["teleport_coords"][0];
            v.teleportCoords.y = (*iter)["teleport_coords"][1];
            v.cameraCoords.x = (*iter)["camera_coords"][0];
            v.cameraCoords.y = (*iter)["camera_coords"][1];
            v.y = (*iter)["y"];
            v.resetLocation.x = (*iter)["reset_location"][0];
            v.resetLocation.y = (*iter)["reset_location"][1];
            v.cameraMax = (*iter)["camera_max"];
            v.bgColor = convertStrToColor((*iter)["bg_color"]);
            v.levelType = convertStrToLevelType((*iter)["level_type"]);
            m_vines.push_back(v);
        }
    }

    if (data["PLATFORM_LEVEL"].is_array()) {
        for (auto iter = data["PLATFORM_LEVEL"].begin(); iter != data["PLATFORM_LEVEL"].end();
             ++iter) {
            PlatformLevel p;
            p.leftCoords.x = (*iter)["left_coords"][0];
            p.leftCoords.y = (*iter)["left_coords"][1];
            p.rightCoords.x = (*iter)["right_coords"][0];
            p.rightCoords.y = (*iter)["right_coords"][1];
            p.yLevel = (*iter)["yLevel"];
            m_platformLevels.push_back(p);
        }
    }
}

sf::Color LevelData::convertStrToColor(std::string colorStr) const
{
    if (colorStr == "BLUE")
    {
        return GLOBALS::BACKGROUND_BLUE_COLOR;
    }
    else if (colorStr == "BLACK")
    {
        return GLOBALS::BACKGROUND_BLACK_COLOR;
    }

    assert(false && "No found colorStr");
    return sf::Color::White;
}

Direction LevelData::convertStrToDirection(std::string directionStr) const
{
    if (directionStr == "NONE")
    {
        return Direction::None;
    }
    else if (directionStr == "LEFT")
    {
        return Direction::Left;
    }
    else if (directionStr == "RIGHT")
    {
        return Direction::Right;
    }
    else if (directionStr == "UP")
    {
        return Direction::Up;
    }
    else if (directionStr == "DOWN")
    {
        return Direction::Down;
    }

    assert(false && "No found directionStr");
    return Direction::None;
}

LevelType LevelData::convertStrToLevelType(std::string levelTypeStr) const
{
    if (levelTypeStr == "NONE")
    {
        return LevelType::None;
    }
    else if (levelTypeStr == "OVERWORLD")
    {
        return LevelType::Overworld;
    }
    else if (levelTypeStr == "CASTLE")
    {
        return LevelType::Castle;
    }
    else if (levelTypeStr == "START_UNDERGROUND")
    {
        return LevelType::StartUnderground;
    }
    else if (levelTypeStr == "UNDERGROUND")
    {
        return LevelType::Underground;
    }
    else if (levelTypeStr == "UNDERWATER")
    {
        return LevelType::Underwater;
    }

    assert(false && "No found levelTypeStr");
    return LevelType::None;
}

PlatformMotionType LevelData::convertStrToPlatformMotionType(std::string platformMotionTypeStr) const
{
    if (platformMotionTypeStr == "NONE") {
        return PlatformMotionType::None;
    } else if (platformMotionTypeStr == "BACK_AND_FORTH") {
        return PlatformMotionType::BackAndForth;
    } else if (platformMotionTypeStr == "GRAVITY") {
        return PlatformMotionType::Gravity;
    } else if (platformMotionTypeStr == "ONE_DIRECTION_CONTINOUS") {
        return PlatformMotionType::OneDirectionContinuous;
    } else if (platformMotionTypeStr == "ONE_DIRECTION_REPEATED") {
        return PlatformMotionType::OneDirectionRepeated;
    }

    assert(false && "No found platformMotionTypeStr");
    return PlatformMotionType::None;
}

RotationDirection LevelData::convertStrToRotationDirection(std::string rotationDirectionStr) const
{
    if (rotationDirectionStr == "COUNTER_CLOCKWISE") {
        return RotationDirection::CounterClockwise;
    } else if (rotationDirectionStr == "CLOCKWISE") {
        return RotationDirection::Clockwise;
    } else {
        return RotationDirection::None;
    }
}

sf::Vector2i LevelData::getPlayerStart() const
{
    return m_playerStart;
}

LevelType LevelData::getLevelType() const
{
    return m_levelType;
}

sf::Vector2i LevelData::getCameraStart() const
{
    return m_cameraStart;
}

sf::Color LevelData::getBackgroundColor() const
{
    return m_backgroundColor;
}

int LevelData::getCameraMax() const
{
    return m_cameraMax;
}

sf::Vector2i LevelData::getNextLevel() const
{
    return m_nextLevel;
}

std::vector<LevelData::Warp> LevelData::getWarpPipes() const
{
    return m_warps;
}

std::vector<LevelData::MovingPlatform> LevelData::getMovingPlatform() const
{
    return m_movingPlatforms;
}

std::vector<LevelData::FloatingText> LevelData::getFloatingTexts() const
{
    return m_floatingTexts;
}

std::vector<LevelData::FireBar> LevelData::getFireBars() const
{
    return m_fireBars;
}

std::vector<LevelData::Vine> LevelData::getVines() const
{
    return m_vines;
}

std::vector<LevelData::PlatformLevel> LevelData::getPlatformLevels() const
{
    return m_platformLevels;
}

void LevelData::setBackgroundColor(sf::Color newColor)
{
    m_backgroundColor = newColor;
}
