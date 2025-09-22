#include "GameWorld.h"
#include "Camera.h"
#include "ECS/systems/AnimationSystem.h"
#include "ECS/systems/CallbackSystem.h"
#include "ECS/systems/CollectibleSystem.h"
#include "ECS/systems/EnemySystem.h"
#include "ECS/systems/FlagSystem.h"
#include "ECS/systems/HUDSystem.h"
#include "ECS/systems/MapSystem.h"
#include "ECS/systems/MusicSystem.h"
#include "ECS/systems/PhysicsSystem.h"
#include "ECS/systems/PlayerSystem.h"
#include "ECS/systems/RenderSystem.h"
#include "ECS/systems/SoundSystem.h"
#include "ECS/systems/WarpSystem.h"
#include "Game.h"
#include "Globals.h"
#include "SharedContext.h"
#include <cstdlib>
#include <iostream>

GameWorld::GameWorld(StateManager *stateManager)
    //: m_sharedContext(stateManager)
    : m_world(new World)
    , m_stateManager(stateManager)
{
    Camera::GetInstance().setCameraX(0);
    Camera::GetInstance().setCameraY(0);

    m_world->registerSystem<MapSystem>();
    setMaxCameraXFromLevelData();
    m_world->registerSystem<RenderSystem>(stateManager->GetContext()->m_window->GetRenderWindow());
    m_world->registerSystem<AnimationSystem>();
    m_world->registerSystem<HUDSystem>();
    m_world->registerSystem<EnemySystem>();

    d = m_world->getSystem<MapSystem>()->getLevelData();
    m_world->registerSystem<PlayerSystem>(d);
    m_world->registerSystem<PhysicsSystem>();
    m_world->registerSystem<CollectibleSystem>();
    m_world->registerSystem<CallbackSystem>();
    m_world->registerSystem<FlagSystem>(this);
    m_world->registerSystem<WarpSystem>(this);
    m_world->registerSystem<SoundSystem>();
    m_world->registerSystem<MusicSystem>();
    setLevelMusic();
}

GameWorld::~GameWorld()
{
    delete m_world;
}

void GameWorld::HandlePlayerInput(const std::optional<sf::Event> &event)
{
    if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->scancode == sf::Keyboard::Scancode::P && m_world->getSystem<SoundSystem>()) {
            m_world->getSystem<SoundSystem>()->playSound(SoundID::Pause);
        }
    }
    m_world->handleInput(event);
}

void GameWorld::Update(const sf::Time time)
{
    m_world->tick();
}

void GameWorld::Render(sf::RenderWindow& window)
{
    if (m_world->hasSystem<MapSystem>())
        window.clear(d.getBackgroundColor());
    m_world->render();
}

LevelData &GameWorld::getLevelData()
{
    m_world->getSystem<MapSystem>()->getLevelData();
}

bool GameWorld::isGameOver()
{
    m_world->getSystem<PlayerSystem>()->isGameOver();
}

void GameWorld::switchLevel()
{
    m_stateManager->Remove(StateType::Game);
    m_stateManager->SwitchTo(StateType::PreGame);
}

void GameWorld::switchToMenu()
{
    m_stateManager->Remove(StateType::Game);
    m_stateManager->SwitchTo(StateType::MainMenu);
}

void GameWorld::setUnderwater(bool on)
{
    m_world->getSystem<PlayerSystem>()->setUnderwater(on);
}

void GameWorld::setLevelMusic()
{
    LevelType type = d.getLevelType();
    auto musicSystem = m_world->getSystem<MusicSystem>();
    switch (type) {
    case LevelType::Overworld:
    case LevelType::StartUnderground:
        musicSystem->playMusic(MusicID::Overworld);
        break;
    case LevelType::Underground:
        musicSystem->playMusic(MusicID::Underground);
        break;
    case LevelType::Castle:
        musicSystem->playMusic(MusicID::Castle);
        break;
    case LevelType::Underwater:
        musicSystem->playMusic(MusicID::Underwater);
        break;
    }
}

void GameWorld::setLevelMusic(LevelType type)
{
    auto musicSystem = m_world->getSystem<MusicSystem>();
    switch (type) {
    case LevelType::Overworld:
    case LevelType::StartUnderground:
        musicSystem->playMusic(MusicID::Overworld);
        break;
    case LevelType::Underground:
        musicSystem->playMusic(MusicID::Underground);
        break;
    case LevelType::Castle:
        musicSystem->playMusic(MusicID::Castle);
        break;
    case LevelType::Underwater:
        musicSystem->playMusic(MusicID::Underwater);
        break;
    }
}

void GameWorld::setMaxCameraXFromLevelData()
{
    Camera::GetInstance().setCameraMaxX(
        m_world->getSystem<MapSystem>()->getLevelData().getCameraMax() * GLOBALS::SCALED_CUBE_SIZE);
}
