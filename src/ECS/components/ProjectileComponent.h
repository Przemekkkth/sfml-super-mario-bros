#ifndef PROJECTILECOMPONENT_H
#define PROJECTILECOMPONENT_H
#include "Component.h"
#include "../../utils/ResourceIdentifiers.h"

class ProjectileComponent : public Component
{
public:
    ProjectileComponent(ProjectileType type)
        : m_type{type}
    {}

    ProjectileType type() const { return m_type; }

private:
    ProjectileType m_type;
};

#endif // PROJECTILECOMPONENT_H
