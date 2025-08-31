#ifndef MUSICSYSTEM_H
#define MUSICSYSTEM_H
#include "../../utils/ResourceIdentifiers.h"
#include "System.h"
#include <SFML/Audio/Music.hpp>
class World;
class MusicSystem : public System
{
public:
    MusicSystem();
    virtual ~MusicSystem();
    virtual void tick(World *world) override;
    virtual void onAddedToWorld(World *world) override;

    void playMusic(MusicID id);
    void pause();
    void resumeAndCheckMarioStatus();
    void stop();

private:
    sf::Music m_bgMusic;
    sf::Music m_subMusic;
    World *m_world;

    // System interface
public:
    virtual void onRemovedFromWorld(World *world) override;
};

#endif // MUSICSYSTEM_H
