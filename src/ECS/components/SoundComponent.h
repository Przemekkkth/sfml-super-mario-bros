#ifndef SOUNDCOMPONENT_H
#define SOUNDCOMPONENT_H
#include "Component.h"
#include "../../utils/ResourceIdentifiers.h"
class SoundComponent : public Component
{
public:
    SoundComponent(SoundID soundID)
        : m_soundID{soundID}
    {}
private:
    SoundID m_soundID;
};

#endif // SOUNDCOMPONENT_H
