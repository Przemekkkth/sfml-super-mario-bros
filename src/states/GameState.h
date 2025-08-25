#ifndef GAMESTATE_H
#define GAMESTATE_H
#include "../GameWorld.h"
#include "BaseState.h"

class GameState : public BaseState
{
public:
    GameState(StateManager* stateManager);
    ~GameState();

    virtual void OnCreate() override;
    virtual void OnDestroy() override;
    virtual void Activate() override;
    virtual void Deactivate() override;
    virtual void Update(const sf::Time &time) override;
    virtual void Draw() override;
    virtual void HandlePlayerInput(const std::optional<sf::Event> &event) override;
private:
    GameWorld *m_world;
    bool m_isPaused = false;
    bool m_isActivated = false;
    bool m_shouldChangeLvl = false;
};

#endif // GAMESTATE_H
