#include "MenuState.h"
#include "../Camera.h"
#include "../ECS/components/BackgroundComponent.h"
#include "../ECS/components/SpritesheetComponent.h"
#include "../ECS/components/TextureComponent.h"
#include "../ECS/components/TransformComponent.h"
#include "../ECS/systems/MenuSystem.h"
#include "../ECS/systems/OptionsSystem.h"
#include "../ECS/systems/RenderSystem.h"
#include "../GameData.h"
#include "../Globals.h"
#include "StateManager.h"
#include <SFML/Window/Event.hpp>
#include <iostream>

#include "../Game.h"

MenuState::MenuState(StateManager *stateManager)
    : BaseState(stateManager), m_title(m_font), m_hintText(m_font), m_authorName(m_font),
    blinkInterval(sf::seconds(0.35f)), blinkTimer(sf::Time::Zero), isVisible(true), m_world(new World())
{
    Camera::GetInstance().setCameraX(0);
    Camera::GetInstance().setCameraY(0);

    m_menuSystem = m_world->registerSystem<MenuSystem>();
    m_world->registerSystem<RenderSystem>(stateManager->GetContext()->m_window->GetRenderWindow());

    createMenuEntities();
}

MenuState::~MenuState()
{
    delete m_world;
}

void MenuState::OnCreate()
{

}

void MenuState::OnDestroy()
{

}

void MenuState::Activate()
{
}

void MenuState::Deactivate()
{

}

void MenuState::Update(const sf::Time &time)
{
    m_world->tick();

    blinkTimer += time;

    if (blinkTimer >= blinkInterval)
    {
        blinkTimer = sf::Time::Zero;

        isVisible = !isVisible;
    }
}

void MenuState::Draw()
{
    sf::RenderWindow* renderWindow = m_stateMgr->GetContext()->m_window->GetRenderWindow();
    renderWindow->clear(GLOBALS::BACKGROUND_BLUE_COLOR);
    m_world->render();
}

void MenuState::HandlePlayerInput(const std::optional<sf::Event> &event)
{
    m_world->handleInput(event);
    if (m_world->hasSystem<OptionsSystem>())
    {
        if (m_world->getSystem<OptionsSystem>()->isFinished())
        {
            m_world->unregisterSystem<OptionsSystem>();
            m_menuSystem->setEnabled(true);
            m_menuSystem->showMenuText();
        }
        return;
    }

    const auto* keyPressed = event->getIf<sf::Event::KeyPressed>();
    if (keyPressed)
    {
        if (keyPressed->scancode == sf::Keyboard::Scancode::E)
        {
            if (m_menuSystem->levelSelected()) {
                GameData::GAME_TIME = 400;
                GameData::LEVEL = m_world->getSystem<MenuSystem>()->getSelectedLevel();
                GameData::SUB_LEVEL = m_world->getSystem<MenuSystem>()->getSelectedSublevel();
                GameData::LIVES = 4;
                m_stateMgr->Remove(StateType::MainMenu);
                m_stateMgr->SwitchTo(StateType::PreGame);
            } else if (m_menuSystem->optionsSelected()) {
                m_world->registerSystem<OptionsSystem>();
                m_menuSystem->setEnabled(false);
                m_menuSystem->hideMenuText();
            }
        }
    }
}

void MenuState::createMenuEntities()
{
    auto& texture = Game::s_textureHolder->get(TextureID::BlockTileSheet);
    backgroundMap.loadMap("assets/data/MenuBackground/MenuBackground_Background.csv");
    unsigned int levelHeight = backgroundMap.getLevelData().size();
    for (auto i = 0; i < levelHeight; ++i)
    {
        for (auto j = 0; j < backgroundMap.getLevelData()[i].size(); j++)
        {
            int entityID = backgroundMap.getLevelData()[i][j];
            if (entityID != -1)
            {
                Entity* entity = m_world->create();
                entity->addComponent<TransformComponent>(
                    sf::Vector2f(j * GLOBALS::SCALED_CUBE_SIZE, i * GLOBALS::SCALED_CUBE_SIZE));
                entity->addComponent<TextureComponent>(texture);
                entity->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE, GLOBALS::ORIGINAL_CUBE_SIZE, 1, 1, 1, GLOBALS::ORIGINAL_CUBE_SIZE,
                                                           GLOBALS::ORIGINAL_CUBE_SIZE, Map::BlockIDCoordinates.at(entityID));
                entity->addComponent<BackgroundComponent>();
            }
        }
    }
}
