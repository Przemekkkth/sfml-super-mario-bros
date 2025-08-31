#ifndef GAMEWORLD_H
#define GAMEWORLD_H
#include "ECS/World.h"
#include "Globals.h"
#include "LevelData.h"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Event.hpp>
#include <deque>
#include <optional>

class StateManager;
class GameWorld
{
public:
    GameWorld(StateManager *stateManager = nullptr);
    ~GameWorld();
    void HandlePlayerInput(const std::optional<sf::Event>& event);
    void Update(const sf::Time time);
    void Render(sf::RenderWindow& window);
    LevelData &getLevelData();
    bool isGameOver();
    LevelData d;

    void switchLevel();
    void setUnderwater(bool on);
    void setLevelMusic();
    void setLevelMusic(LevelType type);

private:
    void setMaxCameraXFromLevelData();
    //SharedContext* m_sharedContext;
    World* m_world;
    StateManager *m_stateManager;
};

#endif // GAMEWORLD_H
