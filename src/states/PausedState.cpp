#include "PausedState.h"
#include "../Globals.h"
#include "StateManager.h"
#include <iostream>
#include "../ECS/World.h"
#include "../ECS/systems/PausedSystem.h"
#include "../ECS/systems/RenderSystem.h"
#include "../ECS/systems/OptionsSystem.h"

PausedState::PausedState(StateManager *stateManager)
    : BaseState(stateManager), m_world{new World()}
{
    SetTransparent(true);
    m_world->registerSystem<PausedSystem>();
    m_world->registerSystem<RenderSystem>(stateManager->GetContext()->m_window->GetRenderWindow());
}

PausedState::~PausedState()
{

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
    if(m_world->hasSystem<PausedSystem>() && m_world->getSystem<PausedSystem>()->isOptionsOpened())
    {
        m_world->unregisterSystem<PausedSystem>();
        m_world->registerSystem<OptionsSystem>();
    }

    if(m_world->hasSystem<PausedSystem>() && m_world->getSystem<PausedSystem>()->isMenuOpened())
    {
        GetStateManager()->Remove(StateType::Paused);
        GetStateManager()->Remove(StateType::Game);
        GetStateManager()->SwitchTo(StateType::MainMenu);
    }

    if(m_world->hasSystem<PausedSystem>() && m_world->getSystem<PausedSystem>()->isFinished())
    {
        GetStateManager()->Remove(StateType::Paused);
        GetStateManager()->SwitchTo(StateType::Game);
    }

    if(m_world->hasSystem<OptionsSystem>() && m_world->getSystem<OptionsSystem>()->isFinished())
    {
        m_world->unregisterSystem<OptionsSystem>();
        m_world->registerSystem<PausedSystem>();
    }
}

void PausedState::Draw()
{
    m_world->render();
}

void PausedState::HandlePlayerInput(const std::optional<sf::Event> &event)
{
    m_world->handleInput(event);

    if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
    {

        if (keyPressed->scancode == sf::Keyboard::Scancode::P)
        {

        }
    }
}
