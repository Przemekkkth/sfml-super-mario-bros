#ifndef MENUSTATE_H
#define MENUSTATE_H
#include "BaseState.h"
#include "../ECS/World.h"
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include "../Map.h"

class MenuSystem;
class MenuState : public BaseState
{
public:
    MenuState(StateManager* stateManager);
    virtual ~MenuState();

    virtual void OnCreate() override;
    virtual void OnDestroy() override;
    virtual void Activate() override;
    virtual void Deactivate() override;
    virtual void Update(const sf::Time &time) override;
    virtual void Draw() override;
    virtual void HandlePlayerInput(const std::optional<sf::Event> &event) override;
private:
    void createMenuEntities();
    sf::Text m_title;
    sf::Text m_hintText;
    sf::Text m_authorName;
    sf::Font m_font;
    sf::Time blinkInterval;
    sf::Time blinkTimer;
    sf::Texture m_backgroundTexture;
    sf::Texture m_avatarTexture;
    sf::Texture m_sfmlLogoTexture;
    bool isVisible;

    World *m_world;
    MenuSystem* m_menuSystem;
    Map backgroundMap;

};

#endif // MENUSTATE_H
