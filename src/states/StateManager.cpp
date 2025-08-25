#include "StateManager.h"
#include <iostream>


StateManager::StateManager(SharedContext *shared)
    : m_shared(shared)
{
    RegisterState<MenuState>(StateType::MainMenu);
    RegisterState<GameState>(StateType::Game);
    RegisterState<PreGameState>(StateType::PreGame);
    RegisterState<PausedState>(StateType::Paused);
    RegisterState<GameOverState>(StateType::GameOver);
}

StateManager::~StateManager()
{
    for (auto &state : m_states){
        state.second->OnDestroy();
        delete state.second;
    }
}

void StateManager::Update(const sf::Time &time)
{
    if (m_states.empty())
    {
        return;
    }

    auto topItr = m_states.end();
    --topItr;
    auto& topState = topItr->second;

    if (topState->IsTranscendent() && m_states.size() > 1)
    {
        auto itr = m_states.end();
        do
        {
            --itr;
            if (!itr->second->IsTranscendent())
            {
                break;
            }
        } while (itr != m_states.begin());

        for (; itr != m_states.end(); ++itr)
        {
            itr->second->Update(time);
        }
    }
    else
    {
        topState->Update(time);
    }
}

void StateManager::Draw()
{
    if (m_states.empty())
    {
        return;
    }

    auto topItr = m_states.end();
    --topItr;
    auto& topState = topItr->second;

    if (topState->IsTransparent() && m_states.size() > 1)
    {
        auto itr = m_states.end();
        do
        {
            --itr;
            if (!itr->second->IsTransparent())
            {
                break;
            }
        } while (itr != m_states.begin());

        for (; itr != m_states.end(); ++itr)
        {
            itr->second->Draw();
        }
    }
    else
    {
        topState->Draw();;
    }
}

void StateManager::ProcessRequests()
{
    while (m_toRemove.begin() != m_toRemove.end())
    {
        RemoveState(*m_toRemove.begin());
        m_toRemove.erase(m_toRemove.begin());
    }
}

SharedContext *StateManager::GetContext()
{
    return m_shared;
}

bool StateManager::HasState(const StateType type)
{
    for (auto state = m_states.begin(); state != m_states.end(); ++state)
    {
        if (state->first == type)
        {
            auto removed = std::find(m_toRemove.begin(), m_toRemove.end(), type);
            if (removed == m_toRemove.end())
            {
                return true;
            }
            return false;
        }
    }
    return false;
}

void StateManager::SwitchTo(const StateType type)
{
    auto topItr = m_states.end();
    --topItr;
    auto& topState = topItr->second;

    for (auto state = m_states.begin(); state != m_states.end(); ++state)
    {
        if (state->first == type)
        {
            topState->Deactivate();
            StateType tmp_type = state->first;
            BaseState* tmp_state = state->second;
            m_states.erase(state);
            m_states.emplace_back(tmp_type, tmp_state);
            tmp_state->Activate();

            return;
        }
    }

    // // State with type wasn't found.
    if (!m_states.empty())
    {
        topState->Deactivate();
    }
    CreateState(type);

    topItr = m_states.end();
    --topItr;
    topItr->second->Activate();
}

void StateManager::Remove(const StateType type)
{
    m_toRemove.push_back(type);
}

void StateManager::HandlePlayerInput(const std::optional<sf::Event> &event)
{
    auto topItr = m_states.end();
    --topItr;
    auto& topState = topItr->second;
    topState->HandlePlayerInput(event);
}

void StateManager::CreateState(const StateType type)
{
    auto newState = m_stateFactory.find(type);
    if (newState == m_stateFactory.end())
    {
        return;
    }

    BaseState* state = newState->second();
    m_states.emplace_back(type, state);
    state->OnCreate();
}

void StateManager::RemoveState(const StateType type)
{
    for (auto itr = m_states.begin(); itr != m_states.end(); ++itr)
    {
        if (itr->first == type)
        {
            itr->second->OnDestroy();
            delete itr->second;
            m_states.erase(itr);
            return;
        }
    }
}
