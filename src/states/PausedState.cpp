#include "PausedState.h"
#include "../ECS/World.h"
#include "../ECS/systems/OptionsSystem.h"
#include "../ECS/systems/PausedSystem.h"
#include "../ECS/systems/RenderSystem.h"
#include "../Globals.h"
#include "StateManager.h"

PausedState::PausedState(StateManager *stateManager)
    : BaseState(stateManager), m_world{new World()}
{
    SetTransparent(true);
    m_pausedSystem = m_world->registerSystem<PausedSystem>();
    m_optionsSystem = m_world->registerSystem<OptionsSystem>();
    m_optionsSystem->hide();
    m_optionsSystem->setEnabled(false);
    m_world->registerSystem<RenderSystem>(stateManager->GetContext()->m_window->GetRenderWindow());
}

PausedState::~PausedState()
{
    delete m_world;
}

void PausedState::OnCreate()
{

}

void PausedState::OnDestroy()
{

}

void PausedState::Activate()
{

}

void PausedState::Deactivate()
{

}

void PausedState::Update(const sf::Time &time)
{
    m_world->tick();
    if (m_pausedSystem->isOptionsOpened()) {
        m_pausedSystem->hide();
        m_optionsSystem->show();
        m_pausedSystem->setEnabled(false);
        m_optionsSystem->setEnabled(true);
    }

    if (m_pausedSystem->isMenuOpened()) {
        GetStateManager()->Remove(StateType::Paused);
        GetStateManager()->Remove(StateType::Game);
        GetStateManager()->SwitchTo(StateType::MainMenu);
    }

    if (m_pausedSystem->isFinished()) {
        GetStateManager()->Remove(StateType::Paused);
        GetStateManager()->SwitchTo(StateType::Game);
    }

    if (m_optionsSystem->isFinished()) {
        m_optionsSystem->hide();
        m_pausedSystem->disableOptions();
        m_pausedSystem->show();
        m_pausedSystem->setEnabled(true);
        m_optionsSystem->setEnabled(false);
    }
}

void PausedState::Draw()
{
    m_world->render();
}

void PausedState::HandlePlayerInput(const std::optional<sf::Event> &event)
{
    m_world->handleInput(event);
}
