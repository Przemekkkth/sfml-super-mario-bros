#ifndef TEXTCOMPONENT_H
#define TEXTCOMPONENT_H
#include "Component.h"
#include <SFML/System/String.hpp>

class TextComponent : public Component
{
public:
    TextComponent(sf::String text, unsigned int fontSize, bool followCamera = false, bool visible = true)
        : m_text{text}, m_fontSize{fontSize}, m_followCamera{followCamera}, m_visible{visible}
    {}

    void setText(sf::String text)
    {
        m_text = text;
    }

    sf::String getText()
    {
        return m_text;
    }

    void setFontSize(unsigned int size)
    {
        m_fontSize = size;
    }

    unsigned int getFontSize()
    {
        return m_fontSize;
    }

    void setFollowCamera(bool on)
    {
        m_followCamera = on;
    }

    bool isFollowCamera()
    {
        return m_followCamera;
    }

    void setVisible(bool on)
    {
        m_visible = on;
    }

    bool isVisible()
    {
        return m_visible;
    }
private:
    sf::String m_text;
    unsigned int m_fontSize;
    bool m_followCamera;
    bool m_visible;
};

#endif // TEXTCOMPONENT_H
