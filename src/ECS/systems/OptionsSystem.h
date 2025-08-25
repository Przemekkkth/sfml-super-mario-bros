#ifndef OPTIONSSYSTEM_H
#define OPTIONSSYSTEM_H
#include "System.h"
#include "../Entity.h"


class OptionsSystem : public System
{
public:
    OptionsSystem();

    virtual void onAddedToWorld(World *world) override;
    virtual void tick(World *world) override;
    virtual void handleInput(const std::optional<sf::Event> &event) override;
    virtual void onRemovedFromWorld(World *world) override;
    bool isFinished()
    {
        return m_finished;
    }

private:
    Entity* m_optionsBackground;
    Entity* m_leftKeybindText;
    Entity* m_leftKeyName;
    Entity* m_rightKeybindText;
    Entity* m_rightKeyName;
    Entity* m_jumpKeybindText;
    Entity* m_jumpKeyName;
    Entity* m_duckKeybindText;
    Entity* m_duckKeyName;
    Entity* m_sprintKeybindText;
    Entity* m_sprintKeyName;
    Entity* m_fireballKeybindText;
    Entity* m_fireballKeyName;
    Entity* m_goBackText;

    Entity* m_infoBackground;
    Entity* m_infoTextEnter;
    Entity* m_infoTextEscape;

    Entity* m_cursor;
    Entity* m_keyUnderline;

    bool m_finished = false;
};

#endif // OPTIONSSYSTEM_H
