#ifndef STATEMANAGER_H
#define STATEMANAGER_H
#include <vector>
#include <unordered_map>
#include <functional>

#include "../SharedContext.h"
#include "MenuState.h"
#include "GameState.h"
#include "PausedState.h"
#include "BaseState.h"
#include "GameOverState.h"
#include "PreGameState.h"

enum class StateType{ MainMenu = 1, PreGame, Game, Paused, GameOver };

using StateContainer = std::vector<std::pair<StateType, BaseState*>>;
using TypeContainer = std::vector<StateType>;
using StateFactory = std::unordered_map<StateType, std::function<BaseState*(void)>>;

class StateManager
{
public:
    StateManager(SharedContext* shared);
    ~StateManager();

    void Update(const sf::Time& time);
    void Draw();

    void ProcessRequests();

    SharedContext* GetContext();
    bool HasState(const StateType type);

    void SwitchTo(const StateType type);
    void Remove(const StateType type);

    void HandlePlayerInput(const std::optional<sf::Event>& event);
private:
    void CreateState(const StateType type);
    void RemoveState(const StateType type);

    template<class T>
    void RegisterState(const StateType type){
        m_stateFactory[type] = [this]() -> BaseState*
        {
            return new T(this);
        };
    }

    SharedContext* m_shared;
    StateContainer m_states;
    TypeContainer m_toRemove;
    StateFactory m_stateFactory;
};

#endif // STATEMANAGER_H
