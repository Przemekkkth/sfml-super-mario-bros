#ifndef MENUSYSTEM_H
#define MENUSYSTEM_H
#include "System.h"

class Entity;

class MenuSystem : public System
{
public:
    MenuSystem() = default;

    virtual void onAddedToWorld(World *world) override;
    virtual void tick(World *world) override;
    virtual void handleInput(const std::optional<sf::Event> &event) override;
    virtual void handleInput() override;
    virtual void onRemovedFromWorld(World *world) override;

    int getSelectedLevel();

    int getSelectedSublevel();

    bool levelSelected()
    {
        return m_levelSelect && m_currentFocus == 1;
    }

    bool optionsSelected()
    {
        return m_currentOption == 1;
    }

    void enterLevelSelect();
    void exitLevelSelect();

    void hideMenuText();
    void showMenuText();
private:
    bool m_levelChange = false;
    bool m_underlineChange = false;

    bool m_levelSelect = false;  // If done selecting a level

    int m_selectedLevel = 1;
    int m_selectedSublevel = 1;
    const int m_maxLevel = 8;
    const int m_maxSublevel = 4;

    int m_currentFocus = 0;       // 0 is to select, 1 selecting level
    int m_currentOption = 0;      // 0 is level select, 1 is options
    int m_currentLevelFocus = 0;  // 0 is level, 1 is sublevel

    Entity* m_logo;
    Entity* m_aboutText;
    Entity* m_selectText;  // "Level Select"
    Entity* m_optionsText;

    Entity* m_cursor;

    Entity* m_levelSelectBackground;
    Entity* m_levelSelectText;  // "Select a Level"
    Entity* m_levelNumber;
    Entity* m_underline;
};

#endif // MENUSYSTEM_H
