#ifndef COLLECTIBLECOMPONENT_H
#define COLLECTIBLECOMPONENT_H
#include "Component.h"
#include "../../utils/ResourceIdentifiers.h"

class CollectibleComponent : public Component
{
public:
    CollectibleComponent(CollectibleType type = CollectibleType::None)
        : m_type(type)
    {}

    CollectibleType type() const { return m_type; }

private:
    CollectibleType m_type;
};

#endif // COLLECTIBLECOMPONENT_H
