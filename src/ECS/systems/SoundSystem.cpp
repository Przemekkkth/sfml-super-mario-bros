#include "SoundSystem.h"
#include "../../Game.h"
#include "../World.h"
#include "../components/SoundComponent.h"

SoundSystem::SoundSystem()
    : m_sound(Game::s_soundBufferHolder->get(SoundID::BlockBreak))
{}

void SoundSystem::tick(World *world)
{
    world->find<SoundComponent>([=](Entity *entity) {
        auto soundComponent = entity->getComponent<SoundComponent>();
        m_sound.setBuffer(Game::s_soundBufferHolder->get(soundComponent->getSoundID()));
        m_sound.play();
        world->destroy(entity);
    });
}

void SoundSystem::playSound(SoundID id)
{
    m_sound.setBuffer(Game::s_soundBufferHolder->get(id));
    m_sound.play();
}
