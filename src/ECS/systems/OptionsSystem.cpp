#include "OptionsSystem.h"
#include "../World.h"
#include "../components/TransformComponent.h"
#include "../components/TextureComponent.h"
#include "../components/IconComponent.h"
#include "../components/TextComponent.h"
#include "../../Game.h"
#include "../../utils/ResourceIdentifiers.h"

OptionsSystem::OptionsSystem() {}

void OptionsSystem::onAddedToWorld(World *world)
{
    m_optionsBackground = world->create();
    m_optionsBackground
        ->addComponent<TransformComponent>(sf::Vector2f{4.5f * GLOBALS::SCALED_CUBE_SIZE,
                                                        1.5f * GLOBALS::SCALED_CUBE_SIZE},
                                           sf::Vector2i{16, 12});
    m_optionsBackground->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::OptionsBackground));
    m_optionsBackground->addComponent<IconComponent>();

    m_infoBackground = world->create();
    m_infoBackground
        ->addComponent<TransformComponent>(sf::Vector2f{5.5f * GLOBALS::SCALED_CUBE_SIZE,
                                                        10.0f * GLOBALS::SCALED_CUBE_SIZE},
                                           sf::Vector2i{14, 3});
    m_infoBackground->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::OptionsInfoBackground))->setVisible(false);
    m_infoBackground->addComponent<IconComponent>();

    m_leftKeybindText = world->create();
    m_leftKeybindText->addComponent<TransformComponent>(
        sf::Vector2f{6.f * GLOBALS::SCALED_CUBE_SIZE, 2.f * GLOBALS::SCALED_CUBE_SIZE});
    m_leftKeybindText->addComponent<TextComponent>(sf::String("LEFT KEY:"), 16);

    m_leftKeyName = world->create();
    m_leftKeyName->addComponent<TransformComponent>(
        sf::Vector2f{14.f * GLOBALS::SCALED_CUBE_SIZE, 2.f * GLOBALS::SCALED_CUBE_SIZE});
    m_leftKeyName->addComponent<TextComponent>("LEFT ARROW", 16);

    m_rightKeybindText = world->create();
    m_rightKeybindText->addComponent<TransformComponent>(
        sf::Vector2f{6.f * GLOBALS::SCALED_CUBE_SIZE, 3.f * GLOBALS::SCALED_CUBE_SIZE});
    m_rightKeybindText->addComponent<TextComponent>(sf::String("RIGHT KEY:"), 16);

    m_rightKeyName = world->create();
    m_rightKeyName->addComponent<TransformComponent>(
        sf::Vector2f{14.f * GLOBALS::SCALED_CUBE_SIZE, 3.f * GLOBALS::SCALED_CUBE_SIZE});
    m_rightKeyName->addComponent<TextComponent>("RIGHT ARROW", 16);

    m_jumpKeybindText = world->create();
    m_jumpKeybindText->addComponent<TransformComponent>(
        sf::Vector2f{6.f * GLOBALS::SCALED_CUBE_SIZE, 4.f * GLOBALS::SCALED_CUBE_SIZE});
    m_jumpKeybindText->addComponent<TextComponent>(sf::String("JUMP KEY:"), 16);

    m_jumpKeyName = world->create();
    m_jumpKeyName->addComponent<TransformComponent>(
        sf::Vector2f{14.f * GLOBALS::SCALED_CUBE_SIZE, 4.f * GLOBALS::SCALED_CUBE_SIZE});
    m_jumpKeyName->addComponent<TextComponent>("SPACE", 16);

    m_duckKeybindText = world->create();
    m_duckKeybindText->addComponent<TransformComponent>(
        sf::Vector2f{6.f * GLOBALS::SCALED_CUBE_SIZE, 5.f * GLOBALS::SCALED_CUBE_SIZE});
    m_duckKeybindText->addComponent<TextComponent>(sf::String("DUCK KEY:"), 16);

    m_duckKeyName = world->create();
    m_duckKeyName->addComponent<TransformComponent>(
        sf::Vector2f{14.f * GLOBALS::SCALED_CUBE_SIZE, 5.f * GLOBALS::SCALED_CUBE_SIZE});
    m_duckKeyName->addComponent<TextComponent>("S", 16);

    m_sprintKeybindText = world->create();
    m_sprintKeybindText->addComponent<TransformComponent>(
        sf::Vector2f{6.f * GLOBALS::SCALED_CUBE_SIZE, 6.f * GLOBALS::SCALED_CUBE_SIZE});
    m_sprintKeybindText->addComponent<TextComponent>(sf::String("SPRINT KEY:"), 16);

    m_sprintKeyName = world->create();
    m_sprintKeyName->addComponent<TransformComponent>(
        sf::Vector2f{14.f * GLOBALS::SCALED_CUBE_SIZE, 6.f * GLOBALS::SCALED_CUBE_SIZE});
    m_sprintKeyName->addComponent<TextComponent>("Shift", 16);

    m_fireballKeybindText = world->create();
    m_fireballKeybindText->addComponent<TransformComponent>(
        sf::Vector2f{6.f * GLOBALS::SCALED_CUBE_SIZE, 7.f * GLOBALS::SCALED_CUBE_SIZE});
    m_fireballKeybindText->addComponent<TextComponent>(sf::String("FIREBALL KEY:"), 16);

    m_fireballKeyName = world->create();
    m_fireballKeyName->addComponent<TransformComponent>(
        sf::Vector2f{14.f * GLOBALS::SCALED_CUBE_SIZE, 7.f * GLOBALS::SCALED_CUBE_SIZE});
    m_fireballKeyName->addComponent<TextComponent>("Q", 16);

    m_goBackText = world->create();
    m_goBackText->addComponent<TransformComponent>(
        sf::Vector2f{6.f * GLOBALS::SCALED_CUBE_SIZE, 9.f * GLOBALS::SCALED_CUBE_SIZE});
    m_goBackText->addComponent<TextComponent>("GO BACK (BACKSPACE)", 16);
}

void OptionsSystem::tick(World *world)
{

}

void OptionsSystem::handleInput(const std::optional<sf::Event> &event)
{
    const auto* keyPressed = event->getIf<sf::Event::KeyPressed>();
    if (keyPressed)
    {
        if (keyPressed->scancode == sf::Keyboard::Scancode::Backspace)
        {
            m_finished = true;
        }
    }
}

void OptionsSystem::onRemovedFromWorld(World *world)
{
    world->destroy(m_optionsBackground);
    world->destroy(m_infoBackground);
    world->destroy(m_leftKeybindText);
    world->destroy(m_leftKeyName);
    world->destroy(m_rightKeybindText);
    world->destroy(m_rightKeyName);
    world->destroy(m_jumpKeybindText);
    world->destroy(m_jumpKeyName);
    world->destroy(m_duckKeybindText);
    world->destroy(m_duckKeyName);
    world->destroy(m_sprintKeybindText);
    world->destroy(m_sprintKeyName);
    world->destroy(m_fireballKeybindText);
    world->destroy(m_fireballKeyName);
    world->destroy(m_goBackText);
}

void OptionsSystem::hide()
{
    m_optionsBackground->getComponent<TextureComponent>()->setVisible(false);
    m_infoBackground->getComponent<TextureComponent>()->setVisible(false);
    m_leftKeybindText->getComponent<TextComponent>()->setVisible(false);
    m_leftKeyName->getComponent<TextComponent>()->setVisible(false);
    m_rightKeybindText->getComponent<TextComponent>()->setVisible(false);
    m_rightKeyName->getComponent<TextComponent>()->setVisible(false);
    m_jumpKeybindText->getComponent<TextComponent>()->setVisible(false);
    m_jumpKeyName->getComponent<TextComponent>()->setVisible(false);
    m_duckKeybindText->getComponent<TextComponent>()->setVisible(false);
    m_duckKeyName->getComponent<TextComponent>()->setVisible(false);
    m_sprintKeybindText->getComponent<TextComponent>()->setVisible(false);
    m_sprintKeyName->getComponent<TextComponent>()->setVisible(false);
    m_fireballKeybindText->getComponent<TextComponent>()->setVisible(false);
    m_fireballKeyName->getComponent<TextComponent>()->setVisible(false);
    m_goBackText->getComponent<TextComponent>()->setVisible(false);
}

void OptionsSystem::show()
{
    m_optionsBackground->getComponent<TextureComponent>()->setVisible(true);
    m_infoBackground->getComponent<TextureComponent>()->setVisible(true);
    m_leftKeybindText->getComponent<TextComponent>()->setVisible(true);
    m_leftKeyName->getComponent<TextComponent>()->setVisible(true);
    m_rightKeybindText->getComponent<TextComponent>()->setVisible(true);
    m_rightKeyName->getComponent<TextComponent>()->setVisible(true);
    m_jumpKeybindText->getComponent<TextComponent>()->setVisible(true);
    m_jumpKeyName->getComponent<TextComponent>()->setVisible(true);
    m_duckKeybindText->getComponent<TextComponent>()->setVisible(true);
    m_duckKeyName->getComponent<TextComponent>()->setVisible(true);
    m_sprintKeybindText->getComponent<TextComponent>()->setVisible(true);
    m_sprintKeyName->getComponent<TextComponent>()->setVisible(true);
    m_fireballKeybindText->getComponent<TextComponent>()->setVisible(true);
    m_fireballKeyName->getComponent<TextComponent>()->setVisible(true);
    m_goBackText->getComponent<TextComponent>()->setVisible(true);
}
