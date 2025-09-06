#ifndef PAUSEDSYSTEM_H
#define PAUSEDSYSTEM_H
#include "System.h"

class Entity;
class World;
class PausedSystem : public System
{
public:
    PausedSystem();
    virtual void onAddedToWorld(World *world) override;
    virtual void tick(World *world) override;
    virtual void handleInput(const std::optional<sf::Event> &event) override;
    virtual void onRemovedFromWorld(World *world) override;
    bool isOptionsOpened() const;
    bool isMenuOpened() const;
    bool isFinished() const;
    void hide();
    void show();
    void disableOptions();

private:
    Entity* m_selectCursor, *m_pausedText, *m_continueText, *m_optionsText, *m_endText;
    int m_currentIndex;
    bool m_isOptionsOpened;
    bool m_isMenuOpened;
    bool m_isFinished;
};

#endif // PAUSEDSYSTEM_H
