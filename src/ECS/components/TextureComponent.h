#ifndef TEXTURECOMPONENT_H
#define TEXTURECOMPONENT_H
#include "Component.h"
#include <SFML/Graphics/Texture.hpp>

class TextureComponent : public Component
{
public:
    TextureComponent(sf::Texture& tex)
        : m_tex{tex}, m_horizontalFlipped{false}, m_verticalFlipped{false}, m_visible{true}
    {
    }

    sf::Rect<int> getSourceRect()
    {
        return m_sourceRect;
    }

    void setHorizontalFlipped(bool val)
    {
        m_horizontalFlipped = val;
    }

    bool isHorizontalFlipped() const { return m_horizontalFlipped; }

    void setVerticalFlipped(bool val)
    {
        m_verticalFlipped = val;
    }

    bool isVerticalFlipped() const { return m_verticalFlipped; }

    void setVisible(bool val)
    {
        m_visible = val;
    }

    bool isHorizontalFlipped()
    {
        return m_horizontalFlipped;
    }

    bool isVerticalFlipped()
    {
        return m_verticalFlipped;
    }

    bool isVisible()
    {
        return m_visible;
    }

    sf::Texture& getTexture()
    {
        return m_tex;
    }

    sf::Vector2u getTexSize()
    {
        return m_tex.getSize();
    }
private:
    sf::Rect<int> m_sourceRect;
    sf::Texture m_tex;
    bool m_horizontalFlipped;
    bool m_verticalFlipped;
    bool m_visible;

};

#endif // TEXTURECOMPONENT_H

