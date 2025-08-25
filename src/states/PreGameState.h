#ifndef PREGAMESTATE_H
#define PREGAMESTATE_H
#include "BaseState.h"
#include "../ECS/World.h"

class PreGameState : public BaseState
{
public:
    PreGameState(StateManager* stateManager);
    ~PreGameState();
    virtual void OnCreate() override;
    virtual void OnDestroy() override;
    virtual void Activate() override;
    virtual void Deactivate() override;
    virtual void Update(const sf::Time &time) override;
    virtual void Draw() override;
    virtual void HandlePlayerInput(const std::optional<sf::Event> &event) override;
protected:
    World* m_world;
    const float timeOfState;
};

#endif // PREGAMESTATE_H
