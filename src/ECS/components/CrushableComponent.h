#ifndef CRUSHABLECOMPONENT_H
#define CRUSHABLECOMPONENT_H
#include "Component.h"
#include <functional>

class Entity;
class CrushableComponent : public Component
{
public:
    CrushableComponent(std::function<void(Entity*)> whenCrushed)
        : m_whenCrushed{whenCrushed} {}

    std::function<void(Entity *)> getWhenCrushed() const { return m_whenCrushed; }

private:
    std::function<void(Entity*)> m_whenCrushed;
};

#endif // CRUSHABLECOMPONENT_H
