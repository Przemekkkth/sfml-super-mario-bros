#ifndef ENEMYCOMPONENT_H
#define ENEMYCOMPONENT_H
#include "Component.h"
#include "../../utils/ResourceIdentifiers.h"

class EnemyComponent : public Component
{
public:
    EnemyComponent(EnemyType type)
        : m_enemyType{type}
    {}

    EnemyType type() const { return m_enemyType; }
    void setType(EnemyType type) { m_enemyType = type; }

private:
    EnemyType m_enemyType;
};

#endif // ENEMYCOMPONENT_H
