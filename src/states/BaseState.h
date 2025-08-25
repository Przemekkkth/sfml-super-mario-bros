#ifndef BASESTATE_H
#define BASESTATE_H

#include <SFML/Graphics.hpp>
class StateManager;

class BaseState
{
    friend class StateManager;
public:
    BaseState(StateManager* stateManager)
        : m_stateMgr(stateManager), m_transparent(false), m_transcendent(false)
    {}

    BaseState(const BaseState&) = delete;
    BaseState &operator =(const BaseState&) = delete;

    virtual ~BaseState(){}

    virtual void OnCreate() = 0;
    virtual void OnDestroy() = 0;

    virtual void Activate() = 0;
    virtual void Deactivate() = 0;

    virtual void Update(const sf::Time& time) = 0;
    virtual void Draw() = 0;

    virtual void HandlePlayerInput(const std::optional<sf::Event>& event)
    {

    }

    void SetTransparent(const bool transparent)
    {
        m_transparent = transparent;
    }

    bool IsTransparent() const
    {
        return m_transparent;
    }

    void SetTranscendent(const bool transcendence)
    {
        m_transcendent = transcendence;
    }

    bool IsTranscendent() const
    {
        return m_transcendent;
    }

    StateManager* GetStateManager()
    {
        return m_stateMgr;
    }
protected:
    StateManager* m_stateMgr;
    bool m_transparent;
    bool m_transcendent;
};

#endif // BASESTATE_H
