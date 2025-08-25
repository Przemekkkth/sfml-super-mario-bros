#ifndef PLATFORMLEVELCOMPONENT_H
#define PLATFORMLEVELCOMPONENT_H
#include "Component.h"

class Entity;
class PlatformLevelComponent : public Component
{
public:
    PlatformLevelComponent(Entity* other, Entity* pulleyLine, int pulleyHeight)
        : m_otherPlatform{other}, m_pulleyLine{pulleyLine}, m_pulleyHeight{pulleyHeight}
    {}

    Entity* getOtherPlatform() const
    {
        return m_otherPlatform;
    }

    Entity *getPulleyLine() { return m_pulleyLine; }

    std::vector<Entity *> &getPulleyLines() { return m_pulleyLines; }

    int &getPulleyHeight() { return m_pulleyHeight; }

private:
    Entity* m_otherPlatform;

    Entity* m_pulleyLine;
    std::vector<Entity*> m_pulleyLines;

    int m_pulleyHeight;
};

#endif // PLATFORMLEVELCOMPONENT_H
