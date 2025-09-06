#include "PausedSystem.h"
#include "../Entity.h"
#include "../components/TransformComponent.h"
#include "../components/TextComponent.h"
#include "../World.h"
#include "../../Globals.h"
#include "OptionsSystem.h"

PausedSystem::PausedSystem()
    : m_currentIndex(0)
    , m_isOptionsOpened(false)
    , m_isMenuOpened(false)
    , m_isFinished(false)
{}

void PausedSystem::onAddedToWorld(World *world)
{
    m_pausedText = world->create();
    m_pausedText->addComponent<TransformComponent>(
        sf::Vector2f(10.8f * GLOBALS::SCALED_CUBE_SIZE, 4.f * GLOBALS::SCALED_CUBE_SIZE));
    m_pausedText->addComponent<TextComponent>("PAUSED", 18, false);

    m_continueText = world->create();
    m_continueText->addComponent<TransformComponent>(
        sf::Vector2f(10.2f * GLOBALS::SCALED_CUBE_SIZE, 8.5f * GLOBALS::SCALED_CUBE_SIZE));
    m_continueText->addComponent<TextComponent>("CONTINUE", 14, false);

    m_optionsText = world->create();
    m_optionsText->addComponent<TransformComponent>(
        sf::Vector2f(10.2f * GLOBALS::SCALED_CUBE_SIZE, 9.5f * GLOBALS::SCALED_CUBE_SIZE));
    m_optionsText->addComponent<TextComponent>("OPTIONS", 14, false);

    m_endText = world->create();
    m_endText->addComponent<TransformComponent>(
        sf::Vector2f(10.2f * GLOBALS::SCALED_CUBE_SIZE, 10.5f * GLOBALS::SCALED_CUBE_SIZE));
    m_endText->addComponent<TextComponent>("END", 14, false);

    m_selectCursor = world->create();
    m_selectCursor->addComponent<TransformComponent>(
        sf::Vector2f(9.5f * GLOBALS::SCALED_CUBE_SIZE, 8.5f * GLOBALS::SCALED_CUBE_SIZE));
    m_selectCursor->addComponent<TextComponent>(">", 14, false);
}

void PausedSystem::tick(World *world)
{

}

void PausedSystem::handleInput(const std::optional<sf::Event> &event)
{
    const auto *keyPressed = event->getIf<sf::Event::KeyPressed>();
    if (keyPressed) {
        if (keyPressed->scancode == sf::Keyboard::Scancode::Up && !m_isOptionsOpened) {
            --m_currentIndex;
            if (m_currentIndex < 0) {
                m_currentIndex = 0;
            }
        }
        if (keyPressed->scancode == sf::Keyboard::Scancode::Down && !m_isOptionsOpened) {
            ++m_currentIndex;
            if (m_currentIndex > 2) {
                m_currentIndex = 2;
            }
        }

        m_selectCursor->getComponent<TransformComponent>()->setTop((8.5f + m_currentIndex)
                                                                   * GLOBALS::SCALED_CUBE_SIZE);

        if ((keyPressed->scancode == sf::Keyboard::Scancode::Enter
             || keyPressed->scancode == sf::Keyboard::Scancode::E)
            && m_currentIndex == 0) {
            m_isFinished = true;
        }

        if ((keyPressed->scancode == sf::Keyboard::Scancode::Enter
             || keyPressed->scancode == sf::Keyboard::Scancode::E)
            && m_currentIndex == 1) {
            m_isOptionsOpened = true;
        }

        if ((keyPressed->scancode == sf::Keyboard::Scancode::Enter
             || keyPressed->scancode == sf::Keyboard::Scancode::E)
            && m_currentIndex == 2) {
            m_isMenuOpened = true;
        }
    }
}

bool PausedSystem::isOptionsOpened() const
{
    return m_isOptionsOpened;
}

bool PausedSystem::isMenuOpened() const
{
    return m_isMenuOpened;
}

bool PausedSystem::isFinished() const
{
    return m_isFinished;
}

void PausedSystem::onRemovedFromWorld(World *world)
{
    world->destroy(m_pausedText);
    world->destroy(m_continueText);
    world->destroy(m_optionsText);
    world->destroy(m_endText);
    world->destroy(m_selectCursor);
}
