#ifndef ADDLIVESCOMPONENT_H
#define ADDLIVESCOMPONENT_H
#include "Component.h"

class AddLivesComponent : public Component
{
public:
    AddLivesComponent(int lives)
        : m_lives{lives}
    {}
private:
    int m_lives;
};

#endif // ADDLIVESCOMPONENT_H
