#ifndef MUSICCOMPONENT_H
#define MUSICCOMPONENT_H
#include "Component.h"
#include "../../utils/ResourceIdentifiers.h"

class MusicComponent : public Component
{
public:
    MusicComponent(MusicID musicID)
        : m_musicID{musicID}
    {}
private:
    MusicID m_musicID;
};

#endif // MUSICCOMPONENT_H
