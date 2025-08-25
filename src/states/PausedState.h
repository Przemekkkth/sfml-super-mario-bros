#ifndef PAUSEDSTATE_H
#define PAUSEDSTATE_H
#include "BaseState.h"

class World;
class PausedState : public BaseState
{
public:
    PausedState(StateManager* stateManager);
    ~PausedState();

    virtual void OnCreate() override;
    virtual void OnDestroy() override;
    virtual void Activate() override;
    virtual void Deactivate() override;
    virtual void Update(const sf::Time &time) override;
    virtual void Draw() override;
    virtual void HandlePlayerInput(const std::optional<sf::Event> &event) override;

private:
    World* m_world;
};

#endif // PAUSEDSTATE_H
