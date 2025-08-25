#include "GameOverState.h"
#include "../ECS/Entity.h"
#include "../ECS/components/TextComponent.h"
#include "../ECS/components/TransformComponent.h"
#include "../ECS/systems/RenderSystem.h"
#include "../commands/CommandScheduler.h"
#include "../commands/RunCommand.h"
#include "../commands/SequenceCommand.h"
#include "../commands/WaitCommand.h"

#include "StateManager.h"
GameOverState::GameOverState(StateManager *stateManager)
    : BaseState(stateManager)
    , m_world(new World())
{
    m_world->registerSystem<RenderSystem>(stateManager->GetContext()->m_window->GetRenderWindow());

    Entity *textEntity = m_world->create();
    textEntity->addComponent<TransformComponent>(
        sf::Vector2f(10.f * GLOBALS::SCALED_CUBE_SIZE, 7 * GLOBALS::SCALED_CUBE_SIZE));
    textEntity->addComponent<TextComponent>("GAME OVER", 16);

    CommandScheduler::getInstance().addCommand(new SequenceCommand{
        std::vector<Command *>{new WaitCommand{2.f}, new RunCommand{[this]() {
                                   this->GetStateManager()->Remove(StateType::GameOver);
                                   this->GetStateManager()->SwitchTo(StateType::MainMenu);
                               }}}});
}

GameOverState::~GameOverState()
{
    delete m_world;
}

void GameOverState::OnCreate() {}

void GameOverState::OnDestroy() {}

void GameOverState::Activate() {}

void GameOverState::Deactivate() {}

void GameOverState::Update(const sf::Time &time)
{

}

void GameOverState::Draw()
{
    m_world->render();
}

void GameOverState::HandlePlayerInput(const std::optional<sf::Event> &event)
{

}
