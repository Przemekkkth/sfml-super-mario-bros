#ifndef GAMEOVERSTATE_H
#define GAMEOVERSTATE_H
#include "../ECS/World.h"
#include "BaseState.h"

class GameOverState : public BaseState
{
public:
    GameOverState(StateManager* stateManager);
    virtual ~GameOverState();
    // BaseState interface
public:
    virtual void OnCreate() override;
    virtual void OnDestroy() override;
    virtual void Activate() override;
    virtual void Deactivate() override;
    virtual void Update(const sf::Time &time) override;
    virtual void Draw() override;
    virtual void HandlePlayerInput(const std::optional<sf::Event> &event) override;
private:
    World *m_world;
};

#endif // GAMEOVERSTATE_H
