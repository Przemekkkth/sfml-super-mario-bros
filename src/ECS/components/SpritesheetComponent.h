#ifndef SPRITESHEETCOMPONENT_H
#define SPRITESHEETCOMPONENT_H
#include "Component.h"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

class SpritesheetComponent : public Component
{
public:
    SpritesheetComponent(int entityWidth, int entityHeight, int xOffset,
                         int yOffset, int gridGapWidth, int gridWidth,
                         int gridHeight, sf::Vector2i spritesheetCoordinates)
        : m_entityWidth{entityWidth}, m_entityHeight{entityHeight}, m_xOffset{xOffset},
        m_yOffset{yOffset}, m_gridGapWidth{gridGapWidth}, m_gridWidth{gridWidth},
        m_gridHeight{gridHeight}, m_spritesheetCoordinates{spritesheetCoordinates}
    {
        m_sourceRect.position.x = xOffset + ((spritesheetCoordinates.x) * gridGapWidth) +
                       ((spritesheetCoordinates.x) * gridWidth);
        m_sourceRect.position.y = yOffset + ((spritesheetCoordinates.y) * gridGapWidth) +
                       ((spritesheetCoordinates.y) * gridHeight);
        m_sourceRect.size.x = entityWidth;
        m_sourceRect.size.y = entityHeight;
    }

    void setSpritesheetCoordinates(sf::Vector2i coords)
    {
        m_spritesheetCoordinates = coords;
        m_sourceRect.position.x = m_xOffset + ((m_spritesheetCoordinates.x) * m_gridGapWidth) +
                       ((m_spritesheetCoordinates.x) * m_gridWidth);
        m_sourceRect.position.y = m_yOffset + ((m_spritesheetCoordinates.y) * m_gridGapWidth) +
                       ((m_spritesheetCoordinates.y) * m_gridHeight);
    }

    void setSpritesheetXCoordinate(int xCoordinate)
    {
        m_spritesheetCoordinates.x = xCoordinate;

        m_sourceRect.position.x = m_xOffset + ((m_spritesheetCoordinates.x) * m_gridGapWidth) +
                       ((m_spritesheetCoordinates.x) * m_gridWidth);
    }

    void setEntityHeight(int newEntityHeight)
    {
        m_entityHeight = newEntityHeight;
        m_sourceRect.size.y = m_entityHeight;
    }

    void setEntityWidth(int newEntityWidth)
    {
        m_entityWidth = newEntityWidth;
        m_sourceRect.size.x = m_entityWidth;
    }

    void setGridHeight(int newGridHeight)
    {
        m_gridHeight = newGridHeight;

        m_sourceRect.position.y = m_yOffset + ((m_spritesheetCoordinates.y) * m_gridGapWidth) +
                       ((m_spritesheetCoordinates.y) * m_gridHeight);
    }

    sf::Vector2i getSpritesheetCoordinates()
    {
        return m_spritesheetCoordinates;
    }

    sf::Rect<int>& getSourceRect()
    {
        return m_sourceRect;
    }
private:
    sf::Rect<int> m_sourceRect;

    int m_entityWidth;
    int m_entityHeight;
    int m_xOffset;
    int m_yOffset;
    int m_gridGapWidth;
    int m_gridWidth;
    int m_gridHeight;
    sf::Vector2i m_spritesheetCoordinates;
};

#endif // SPRITESHEETCOMPONENT_H
