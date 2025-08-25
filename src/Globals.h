#ifndef GLOBALS_H
#define GLOBALS_H
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/String.hpp>

namespace GLOBALS {
    const unsigned int ORIGINAL_CUBE_SIZE = 16;
    const unsigned int CUBE_SCALE_FACTOR = 2;
    const unsigned int SCALED_CUBE_SIZE = ORIGINAL_CUBE_SIZE * CUBE_SCALE_FACTOR;
    const unsigned int WINDOW_WIDTH = 25*SCALED_CUBE_SIZE;
    const unsigned int WINDOW_HEIGHT = 15*SCALED_CUBE_SIZE;
    const sf::Vector2u WINDOW_SIZE(WINDOW_WIDTH, WINDOW_HEIGHT);
    const sf::String WINDOW_TITLE("SFML Super Mario Bros");
    const unsigned int FPS = 60;
    const sf::Color BACKGROUND_BLUE_COLOR = sf::Color(136, 188, 240);
    const sf::Color BACKGROUND_BLACK_COLOR = sf::Color(0, 0, 0);
    const float GRAVITY = 0.575f;
    const float FRICTION = 0.94f;
    const float MAX_SPEED_X = 10.0f;
    const float MAX_SPEED_Y = 7.5f;
    const float MAX_UNDERWATER_Y = 2.0f;
    const float MARIO_ACCELERATION_X = 0.24;
    const float MARIO_JUMP_ACCELERATION = 1.1f;
    const float MARIO_BOUNCE = 3.5f;
    const float ENEMY_BOUNCE = 6.0f; //when jumping on top of enemies
    const float ENEMY_SPEED = 1.0f;
    const float COLLECTIBLE_SPEED = 2.0f;
    const float PROJECTILE_SPEED = 10.0f;
    const float PROJECTILE_BOUNCE = 4.0f;
    const int TILE_ROUNDNESS = 4;
}

#endif // GLOBALS_H
