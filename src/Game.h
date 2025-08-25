#ifndef GAME_H
#define GAME_H
#include <SFML/System/Time.hpp>
#include <SFML/System/Clock.hpp>
#include "SharedContext.h"
#include "states/StateManager.h"
#include "Window.h"
#include "GameWorld.h"

class Game {
public:
    Game();
    ~Game();

    void HandleInput();
    void Update();
    void Render();
    void LateUpdate();

    sf::Time GetElapsed();
    void RestartClock();

    Window* GetWindow();
    static TextureHolder* s_textureHolder;
    static FontHolder* s_fontHolder;
private:
    void loadTextures();
    void loadFonts();
    void loadMaps();
    SharedContext m_context;
    Window m_window;

    StateManager m_stateManager;
    //GameWorld m_world;
    sf::Clock m_clock;
    float m_elapsed;
};


#endif // GAME_H
