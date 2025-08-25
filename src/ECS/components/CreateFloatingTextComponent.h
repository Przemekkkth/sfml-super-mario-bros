#ifndef CREATEFLOATINGTEXTCOMPONENT_H
#define CREATEFLOATINGTEXTCOMPONENT_H
#include "Component.h"
#include <SFML/System/String.hpp>

class Entity;
class CreateFloatingTextComponent : public Component
{
public:
    CreateFloatingTextComponent(Entity* originalEntity, sf::String text)
        : m_originalEntity{originalEntity}, m_text{text}
    {}

    void setText(sf::String text)
    {
        m_text = text;
    }

    sf::String getText() const
    {
        return m_text;
    }

    void setOriginalEntity(Entity* entity)
    {
        m_originalEntity = entity;
    }

    Entity* getOriginalEntity() const
    {
        return m_originalEntity;
    }
private:
    Entity* m_originalEntity;
    sf::String m_text;
};

#endif // CREATEFLOATINGTEXTCOMPONENT_H

