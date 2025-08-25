#include "PreGameState.h"
#include "../states/StateManager.h"
#include "../ECS/systems/GameplayStatsSystem.h"
#include "../ECS/systems/RenderSystem.h"
#include "../ECS/systems/AnimationSystem.h"
#include "../commands/CommandScheduler.h"
#include "../commands/RunCommand.h"
#include "../commands/WaitCommand.h"
#include "../commands/SequenceCommand.h"

PreGameState::PreGameState(StateManager *stateManager)
    : BaseState(stateManager), m_world(new World()), timeOfState(2.5f)
{
    m_world->registerSystem<GameplayStatsSystem>();
    m_world->registerSystem<RenderSystem>(stateManager->GetContext()->m_window->GetRenderWindow());
    m_world->registerSystem<AnimationSystem>();
}

PreGameState::~PreGameState()
{
    delete m_world;
}

void PreGameState::OnCreate()
{

}

void PreGameState::OnDestroy()
{

}

void PreGameState::Activate()
{
    CommandScheduler::getInstance().addCommand(new SequenceCommand{
        std::vector<Command *>{new WaitCommand{timeOfState}, new RunCommand{[this]() {
                                   this->GetStateManager()->Remove(StateType::PreGame);
                                   this->GetStateManager()->SwitchTo(StateType::Game);
                               }}}});
}

void PreGameState::Deactivate()
{

}

void PreGameState::Update(const sf::Time &time)
{
    m_world->tick();
}

void PreGameState::Draw()
{
    m_world->render();
}

void PreGameState::HandlePlayerInput(const std::optional<sf::Event> &event)
{

}
