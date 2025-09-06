#include "GameState.h"
#include "../Game.h"
#include "../GameData.h"
#include "../Globals.h"
#include "../commands/Command.h"
#include "../commands/CommandScheduler.h"
#include "../commands/RunCommand.h"
#include "../commands/SequenceCommand.h"
#include "../commands/WaitCommand.h"
#include "StateManager.h"
#include <SFML/Window/Event.hpp>

GameState::GameState(StateManager *stateManager)
    : BaseState(stateManager)
{

}

GameState::~GameState()
{

}

void GameState::OnCreate()
{
    m_world = new GameWorld(GetStateManager());
}

void GameState::OnDestroy()
{
    delete m_world;
}

void GameState::Activate()
{
    m_isActivated = true;
}

void GameState::Deactivate()
{

}

void GameState::Update(const sf::Time &time)
{
    if (!m_isActivated) {
        return;
    }
    if (m_world->isGameOver()) {
        if (!m_shouldChangeLvl) {
            GameData::LIVES -= 1;
            m_shouldChangeLvl = true;
            if (m_shouldChangeLvl) {
                CommandScheduler::getInstance().addCommand(new SequenceCommand{
                    std::vector<Command *>{new WaitCommand{3.f}, new RunCommand{[this]() {
                                               this->GetStateManager()->Remove(StateType::Game);
                                               if (GameData::LIVES > 0) {
                                                   this->GetStateManager()->SwitchTo(
                                                       StateType::PreGame);
                                               } else {
                                                   GameData::resetValues();
                                                   this->GetStateManager()->SwitchTo(
                                                       StateType::GameOver);
                                               }
                                           }}}});
            }
        }
    }
    m_world->Update(time);
}

void GameState::Draw()
{
    sf::RenderWindow* window = GetStateManager()->GetContext()->m_window->GetRenderWindow();
    m_world->Render(*window);
}

void GameState::HandlePlayerInput(const std::optional<sf::Event> &event)
{
    m_world->HandlePlayerInput(event);
    if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->scancode == sf::Keyboard::Scancode::P) {
            m_stateMgr->SwitchTo(StateType::Paused);
        }
    }
}
