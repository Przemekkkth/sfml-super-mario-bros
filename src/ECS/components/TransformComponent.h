#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H
#include "Component.h"
#include "../../Globals.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

class TransformComponent : public Component
{
public:
    TransformComponent(sf::Vector2f position, sf::Vector2i scale, sf::Rect<int> hitbox)
        : m_position{position}
        , m_size{scale}
        , m_hitbox{hitbox}
    {
    }

    TransformComponent(sf::Vector2f position, sf::Vector2i scale)
        : m_position{position}
        , m_size{scale}
        , m_hitbox{{0, 0}, {scale.x, scale.y}}
    {
    }

    TransformComponent(sf::Vector2f position)
        : m_position{position}
        , m_size{1, 1}
        , m_hitbox{{0, 0}, {GLOBALS::SCALED_CUBE_SIZE, GLOBALS::SCALED_CUBE_SIZE}}
    {
    }

    float getRight() { return m_position.x + (m_size.x * GLOBALS::SCALED_CUBE_SIZE); }

    float getLeft()
    {
        return m_position.x;
    }

    float getTop()
    {
        return m_position.y;
    }

    float getBottom() { return m_position.y + (m_size.y * GLOBALS::SCALED_CUBE_SIZE); }

    float getCenterX() { return m_position.x + (m_size.x * GLOBALS::SCALED_CUBE_SIZE) / 2.0f; }

    float getCenterY() { return m_position.y + (m_size.y * GLOBALS::SCALED_CUBE_SIZE) / 2.0f; }

    void setTop(float value)
    {
        m_position.y = value;
    }

    void setBottom(float value) { m_position.y = value - (m_size.y * GLOBALS::SCALED_CUBE_SIZE); }

    void setLeft(float value) { m_position.x = value; }

    void setRight(float value) { m_position.x = value - (m_size.x * GLOBALS::SCALED_CUBE_SIZE); }

    void setCenterX(float value)
    {
        m_position.x = value - (m_size.x * GLOBALS::SCALED_CUBE_SIZE) / 2.0f;
    }

    void setCenterY(float value)
    {
        m_position.y = value - (m_size.y * GLOBALS::SCALED_CUBE_SIZE) / 2.0f;
    }

    void setPosition(sf::Vector2f newPosition) { m_position = newPosition; }

    sf::Vector2f getPosition()
    {
        return m_position;
    }

    sf::Vector2i &getSize() { return m_size; }

    sf::Rect<int> &getHitbox() { return m_hitbox; }

    void setHitbox(float x, float y, float w, float h)
    {
        m_hitbox.position.x = x;
        m_hitbox.position.y = y;
        m_hitbox.size.x = w;
        m_hitbox.size.y = h;
    }

    void setHitboxX(float val) { m_hitbox.size.x = val; }

    void move(sf::Vector2f offset)
    {
        m_position += offset;
    }
private:
    sf::Vector2f m_position;
    sf::Vector2i m_size;
    sf::Rect<int> m_hitbox;
};


#endif // TRANSFORMCOMPONENT_H


