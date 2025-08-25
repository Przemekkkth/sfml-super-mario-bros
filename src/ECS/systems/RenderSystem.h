#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H
#include "System.h"
#include <SFML/Graphics/RenderWindow.hpp>

class Entity;
class World;
class RenderSystem : public System
{
public:
    RenderSystem(sf::RenderWindow* window);

    virtual void onAddedToWorld(World *world) override;
    virtual void tick(World *world) override;
    virtual void handleInput(const std::optional<sf::Event> &event) override;
    virtual void handleInput() override;
    virtual void onRemovedFromWorld(World *world) override;

    void setTransitionRendering(bool transition);
    bool isTransitionRendering();
    virtual void render() override;
private:
    void renderEntity(Entity* entity, bool cameraBound = true);
    void renderText(Entity* entity, bool followCamera = false);

    bool m_transitionRendering = false;
    sf::RenderWindow* m_window;
    World* world;
};

#endif // RENDERSYSTEM_H
