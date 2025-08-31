#include "MusicSystem.h"
#include "../../Game.h"
#include "../components/MusicComponent.h"
#include "../components/PlayerComponent.h"

MusicSystem::MusicSystem()
{
    m_bgMusic.setLooping(true);
    m_subMusic.setLooping(false);
}

MusicSystem::~MusicSystem()
{
    stop();
}

void MusicSystem::tick(World *world)
{
    world->find<MusicComponent>([=](Entity *entity) {
        auto music = entity->getComponent<MusicComponent>();
        pause();
        m_subMusic.stop();
        m_subMusic.openFromFile(Game::s_musicStreamNameHolder->get(music->getId()));
        m_subMusic.play();
        world->destroy(entity);
    });

    if (m_bgMusic.getStatus() == sf::SoundStream::Status::Paused
        && m_subMusic.getStatus() == sf::SoundStream::Status::Stopped) {
        m_bgMusic.play();
    }
}

void MusicSystem::onAddedToWorld(World *world)
{
    m_world = world;
}

void MusicSystem::playMusic(MusicID id)
{
    m_bgMusic.stop();
    m_bgMusic.openFromFile(Game::s_musicStreamNameHolder->get(id));
    m_bgMusic.play();
}

void MusicSystem::pause()
{
    m_bgMusic.pause();
    m_subMusic.pause();
}

void MusicSystem::resumeAndCheckMarioStatus()
{
    auto player = m_world->findFirst<PlayerComponent>();
    if (player->getComponent<PlayerComponent>()->isSuperStar()) {
        m_subMusic.play();
    } else {
        m_bgMusic.play();
    }
}

void MusicSystem::stop()
{
    m_bgMusic.stop();
    m_subMusic.stop();
}

void MusicSystem::onRemovedFromWorld(World *world)
{
    stop();
}
