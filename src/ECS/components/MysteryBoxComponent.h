#ifndef MYSTERYBOXCOMPONENT_H
#define MYSTERYBOXCOMPONENT_H
#include "Component.h"
#include "../Entity.h"
#include "../../utils/ResourceIdentifiers.h"
#include <functional>

class MysteryBoxComponent : public Component
{
public:
    MysteryBoxComponent() = default;
    MysteryBoxComponent(MysteryBoxType type = MysteryBoxType::None)
        : m_boxType(type), m_whenDispensed([](Entity* entity){})
    {}
    MysteryBoxComponent(MysteryBoxType type, std::function<void(Entity*)> dispensed)
        : m_boxType(type), m_whenDispensed(dispensed)
    {}
    void setWhenDispensed(std::function<void(Entity *)> f) { m_whenDispensed = f; }

    std::function<void(Entity *)> whenDispensed() const { return m_whenDispensed; }

    void setDeactivatedCoordinates(sf::Vector2i v)
    {
        m_deactivatedCoordinates = v;
    }
    void setBoxType(MysteryBoxType t)
    {
        m_boxType = t;
    }

    MysteryBoxType boxType() const { return m_boxType; }

    sf::Vector2i deactivatedCoordinates() const { return m_deactivatedCoordinates; }

private:
    std::function<void(Entity*)> m_whenDispensed;
    sf::Vector2i m_deactivatedCoordinates;
    MysteryBoxType m_boxType;
};

#endif // MYSTERYBOXCOMPONENT_H
