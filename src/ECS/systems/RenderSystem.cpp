#include "RenderSystem.h"

#include "../Entity.h"
#include "../components/TransformComponent.h"
#include "../components/TextureComponent.h"
#include "../components/BackgroundComponent.h"
#include "../components/ForegroundComponent.h"
#include "../components/ProjectileComponent.h"
#include "../components/CollectibleComponent.h"
#include "../components/EnemyComponent.h"
#include "../components/TextComponent.h"
#include "../components/FloatingTextComponent.h"
#include "../components/PlayerComponent.h"
#include "../components/AboveForegroundComponent.h"
#include "../components/ParticleComponent.h"
#include "../components/IconComponent.h"
#include "../components/SpritesheetComponent.h"
#include "../components/TextComponent.h"

#include "../World.h"
#include "../../Camera.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <cassert>

#include "../../Game.h"

RenderSystem::RenderSystem(sf::RenderWindow *window)
    : m_window(window)
{
    assert(m_window && "There are five lights");
}

void RenderSystem::onAddedToWorld(World *world)
{
    this->world = world;
}

void RenderSystem::tick(World *world)
{

}

void RenderSystem::handleInput(const std::optional<sf::Event> &event)
{

}

void RenderSystem::handleInput()
{

}

void RenderSystem::onRemovedFromWorld(World *world)
{

}

void RenderSystem::setTransitionRendering(bool transition)
{
    m_transitionRendering = transition;
}

bool RenderSystem::isTransitionRendering()
{
    return m_transitionRendering;
}

void RenderSystem::render()
{
    //TextureManager::Get().Clear();
    // This is to render the entities in the correct order
    if (!m_transitionRendering) {  // Don't show the entities being loaded during a transition
        world->find<TransformComponent, TextureComponent, BackgroundComponent>([&](Entity* entity) {
            if (Camera::GetInstance().inCameraRange(entity->getComponent<TransformComponent>()))
            {
                renderEntity(entity);
            }
        });
        world->find<TransformComponent, TextureComponent, ForegroundComponent>([&](Entity* entity) {
            if (Camera::GetInstance().inCameraRange(entity->getComponent<TransformComponent>()))
            {
                renderEntity(entity);
            }
        });
        world->find<TransformComponent, TextureComponent, ProjectileComponent>([&](Entity* entity) {
            renderEntity(entity);
        });
        world->find<TransformComponent, TextureComponent, CollectibleComponent>([&](Entity* entity) {
            if (Camera::GetInstance().inCameraRange(entity->getComponent<TransformComponent>()))
            {
                renderEntity(entity);
            }
        });
        world->find<TransformComponent, TextureComponent, EnemyComponent>([&](Entity* entity) {
            if (Camera::GetInstance().inCameraRange(entity->getComponent<TransformComponent>()))
            {
                renderEntity(entity);
            }
        });
        world->find<TransformComponent, TextComponent, FloatingTextComponent>([&](Entity* entity) {
            renderText(entity, entity->getComponent<TextComponent>()->isFollowCamera());
        });
        world->find<TransformComponent, TextureComponent, PlayerComponent>(
            [&](Entity *entity) { renderEntity(entity); });
        world->find<TransformComponent, TextureComponent, AboveForegroundComponent>(
            [&](Entity* entity) {
                if (Camera::GetInstance().inCameraRange(entity->getComponent<TransformComponent>()))
                {
                    renderEntity(entity);
                }
            });
        world->find<TransformComponent, TextureComponent, ParticleComponent>([&](Entity* entity) {
            renderEntity(entity);
        });
    }

    world->find<TransformComponent, TextureComponent, IconComponent>([&](Entity* entity) {
        renderEntity(entity, false);
    });
    world->find<TransformComponent, TextComponent>([&](Entity* entity) {
        if (!entity->hasComponent<FloatingTextComponent>())
        {
            renderText(entity, entity->getComponent<TextComponent>()->isFollowCamera());
        }
    });

}

void RenderSystem::renderEntity(Entity *entity, bool cameraBound)
{
    auto* positionComponent = entity->getComponent<TransformComponent>();
    auto* textureComponent = entity->getComponent<TextureComponent>();
    if (!textureComponent->isVisible())
    {
        return;
    }

    float screenPositionX =
        (cameraBound) ? positionComponent->getPosition().x - Camera::GetInstance().getCameraX() : positionComponent->getPosition().x;
    float screenPositionY =
        (cameraBound) ? positionComponent->getPosition().y - Camera::GetInstance().getCameraY() : positionComponent->getPosition().y;

    if (entity->hasComponent<SpritesheetComponent>())
    {
        auto* spritesheetComponent = entity->getComponent<SpritesheetComponent>();
        sf::Sprite sprite{textureComponent->getTexture()};

        int x = spritesheetComponent->getSourceRect().position.x;
        int y = spritesheetComponent->getSourceRect().position.y;
        int w = spritesheetComponent->getSourceRect().size.x;
        int h = spritesheetComponent->getSourceRect().size.y;
        sf::IntRect textureRect{{x, y}, {w, h}};
        sf::Vector2i size = entity->getComponent<TransformComponent>()->getSize();

        sprite.setTextureRect(textureRect);
        sprite.setPosition({screenPositionX
                                - (textureComponent->isHorizontalFlipped()
                                       ? -float(GLOBALS::SCALED_CUBE_SIZE) * size.x
                                       : 0.0f),
                            screenPositionY
                                - (textureComponent->isVerticalFlipped()
                                       ? -float(GLOBALS::SCALED_CUBE_SIZE) * size.y
                                       : 0.0f)});

        float mirrorX = textureComponent->isHorizontalFlipped() ? -1.0f : 1.0f;
        float mirrorY = textureComponent->isVerticalFlipped() ? -1.0f : 1.0f;

        const int convertFromSpritesheetToScreenFactor = 2;
        sprite.setScale({convertFromSpritesheetToScreenFactor * mirrorX,
                         convertFromSpritesheetToScreenFactor * mirrorY});
        m_window->draw(sprite);
    }
    else
    {
        sf::Sprite sprite{textureComponent->getTexture()};
        sprite.setPosition({screenPositionX, screenPositionY});
        sf::Vector2 textureSize = textureComponent->getTexture().getSize();

        float scaleX = (positionComponent->getSize().x * GLOBALS::SCALED_CUBE_SIZE)
                       / float(textureSize.x);

        float scaleY = (positionComponent->getSize().y * GLOBALS::SCALED_CUBE_SIZE)
                       / float(textureSize.y);

        sprite.setScale({scaleX, scaleY});
        m_window->draw(sprite);
    }
}


void RenderSystem::renderText(Entity *entity, bool followCamera)
{
    auto* transform = entity->getComponent<TransformComponent>();
    auto* text      = entity->getComponent<TextComponent>();
    float x = transform->getPosition().x;
    float y = transform->getPosition().y;
    if (followCamera)
    {
        x = transform->getPosition().x - Camera::GetInstance().getCameraLeft();
        y = transform->getPosition().y - Camera::GetInstance().getCameraY();
    }

    if (text->isVisible())
    {
        sf::Text t(Game::s_fontHolder->get(FontID::Main));
        t.setString(text->getText());
        t.setCharacterSize(text->getFontSize());
        t.setFillColor(sf::Color::White);
        t.setPosition({x, y});
        m_window->draw(t);
    }
}
