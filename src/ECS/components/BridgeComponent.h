#ifndef BRIDGECOMPONENT_H
#define BRIDGECOMPONENT_H
#include "Component.h"
#include <vector>
class Entity;
class BridgeComponent : public Component
{
public:
    BridgeComponent() = default;
    void setConnectedBridgeParts(std::vector<Entity*>& v)
    {
        m_connectedBridgeParts = v;
    }

    std::vector<Entity *> &getConnectedBridgeParts() { return m_connectedBridgeParts; }

private:
    std::vector<Entity*> m_connectedBridgeParts;
};

#endif // BRIDGECOMPONENT_H
