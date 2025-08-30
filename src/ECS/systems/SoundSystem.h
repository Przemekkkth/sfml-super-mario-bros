#ifndef SOUNDSYSTEM_H
#define SOUNDSYSTEM_H
#include "../../utils/ResourceIdentifiers.h"
#include "System.h"
#include <SFML/Audio/Sound.hpp>

class SoundSystem : public System
{
public:
    SoundSystem();
    virtual void tick(World *world) override;
    void playSound(SoundID id);

private:
    sf::Sound m_sound;
};

#endif // SOUNDSYSTEM_H
