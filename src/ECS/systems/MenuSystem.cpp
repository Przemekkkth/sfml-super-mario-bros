#include "MenuSystem.h"
#include "../World.h"
#include "../components/TransformComponent.h"
#include "../components/TextureComponent.h"
#include "../components/IconComponent.h"
#include "../components/TextComponent.h"
#include "../../Game.h"
#include "../../utils/ResourceIdentifiers.h"

void MenuSystem::onAddedToWorld(World *world)
{
    // LOGO
    m_logo = world->create();
    m_logo->addComponent<TransformComponent>(sf::Vector2f(7.0f * GLOBALS::SCALED_CUBE_SIZE,
                                                          2.0f * GLOBALS::SCALED_CUBE_SIZE),
                                             sf::Vector2i(11, 6));
    m_logo->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::TitleLogo));
    m_logo->addComponent<IconComponent>();

    // LEVEL SELECT BACKGROUND
    m_levelSelectBackground = world->create();
    m_levelSelectBackground
        ->addComponent<TransformComponent>(sf::Vector2f{5.5f * GLOBALS::SCALED_CUBE_SIZE,
                                                        9.5f * GLOBALS::SCALED_CUBE_SIZE},
                                           sf::Vector2i{14, 3});
    m_levelSelectBackground->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::OptionsInfoBackground))->setVisible(false);
    m_levelSelectBackground->addComponent<IconComponent>();

    m_aboutText = world->create();
    m_aboutText->addComponent<TransformComponent>(
        sf::Vector2f{5.5f * GLOBALS::SCALED_CUBE_SIZE, 8.5f * GLOBALS::SCALED_CUBE_SIZE});
    m_aboutText->addComponent<TextComponent>(
        sf::String("Recreated by Gold87 using C++ and SDL2\n\n\tPorted to SFML by Przemekkkth"), 12);

    m_selectText = world->create();
    m_selectText->addComponent<TransformComponent>(
        sf::Vector2f{9.0f * GLOBALS::SCALED_CUBE_SIZE, 10.0f * GLOBALS::SCALED_CUBE_SIZE});
    m_selectText->addComponent<TextComponent>("Level Select", 15);

    m_optionsText = world->create();
    m_optionsText->addComponent<TransformComponent>(
        sf::Vector2f{9 * GLOBALS::SCALED_CUBE_SIZE, 11 * GLOBALS::SCALED_CUBE_SIZE});
    m_optionsText->addComponent<TextComponent>("Options", 15);

    m_levelSelectText = world->create();
    m_levelSelectText->addComponent<TransformComponent>(
        sf::Vector2f{9.5f * GLOBALS::SCALED_CUBE_SIZE, 10.0f * GLOBALS::SCALED_CUBE_SIZE});
    m_levelSelectText->addComponent<TextComponent>(sf::String("Select a Level"), 15, false, false);

    m_levelNumber = world->create();
    m_levelNumber->addComponent<TransformComponent>(
        sf::Vector2f{11.5f * GLOBALS::SCALED_CUBE_SIZE, 11.0f * GLOBALS::SCALED_CUBE_SIZE});
    m_levelNumber->addComponent<TextComponent>(sf::String(std::to_string(m_selectedLevel) + " - " + std::to_string(m_selectedSublevel)), 15, false, false);

    m_underline = world->create();
    m_underline->addComponent<TransformComponent>(
        sf::Vector2f{11.5f * GLOBALS::SCALED_CUBE_SIZE, 11.2f * GLOBALS::SCALED_CUBE_SIZE});
    m_underline->addComponent<TextComponent>("_", 15, false, false);

    m_cursor = world->create();
    m_cursor->addComponent<TransformComponent>(
        sf::Vector2f{8 * GLOBALS::SCALED_CUBE_SIZE, 10 * GLOBALS::SCALED_CUBE_SIZE});
    m_cursor->addComponent<TextComponent>(">", 15);

    Entity *avatar = world->create();
    avatar->addComponent<TransformComponent>(sf::Vector2f(14 * GLOBALS::SCALED_CUBE_SIZE,
                                                          10 * GLOBALS::SCALED_CUBE_SIZE),
                                             sf::Vector2i(4, 4));
    avatar->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::Avatar));
    avatar->addComponent<IconComponent>();

    Entity *sfmlLogo = world->create();
    sfmlLogo->addComponent<TransformComponent>(sf::Vector2f(13 * GLOBALS::SCALED_CUBE_SIZE,
                                                            0.5 * GLOBALS::SCALED_CUBE_SIZE),
                                               sf::Vector2i(4, 4));
    sfmlLogo->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::SFMLLogo));
    sfmlLogo->addComponent<IconComponent>();
}

void MenuSystem::tick(World *world)
{
    if (m_levelChange)
    {

        m_levelNumber->getComponent<TextComponent>()->setText(sf::String(
            std::to_string(m_selectedLevel) + " - " + std::to_string(m_selectedSublevel)));;

        m_levelChange = false;
    }
    if (m_underlineChange)
    {
        if (m_currentLevelFocus == 0)
        {
            m_underline->getComponent<TransformComponent>()->setLeft(11.5f
                                                                     * GLOBALS::SCALED_CUBE_SIZE);
        }
        else if (m_currentLevelFocus == 1)
        {
            m_underline->getComponent<TransformComponent>()->setLeft(13.4f
                                                                     * GLOBALS::SCALED_CUBE_SIZE);
        }
        m_underlineChange = false;
    }
}

void MenuSystem::handleInput(const std::optional<sf::Event> &event)
{
    const auto* keyPressed = event->getIf<sf::Event::KeyPressed>();
    if (keyPressed)
    {
        if (keyPressed->scancode == sf::Keyboard::Scancode::E)
        {
            if (m_currentFocus == 0 && m_currentOption == 0)
            {
                enterLevelSelect();
            } else if (m_currentFocus == 1)
            {
                m_levelSelect = true;
            }
        }

        if (keyPressed->scancode == sf::Keyboard::Scancode::Backspace)
        {
            if (m_currentFocus == 1)
            {
                exitLevelSelect();
            }
        }

        if (keyPressed->scancode == sf::Keyboard::Scancode::Up)
        {
            switch (m_currentFocus)
            {
            case 0:
                if (m_currentOption > 0)
                {
                    m_currentOption--;
                    m_cursor->getComponent<TransformComponent>()->move(sf::Vector2f{0.0f, -static_cast<float>(GLOBALS::SCALED_CUBE_SIZE)});
                }
                break;
            case 1:
                if (m_currentLevelFocus == 0)
                {
                    if (m_selectedLevel < m_maxLevel)
                    {
                        m_selectedLevel++;
                        m_levelChange = true;
                    }
                }
                else if (m_currentLevelFocus == 1)
                {
                    if (m_selectedSublevel < m_maxSublevel)
                    {
                        m_selectedSublevel++;
                        m_levelChange = true;
                    }
                }
                break;
            default:
                break;
            }
        }

        if (keyPressed->scancode == sf::Keyboard::Scancode::Down)
        {
            switch (m_currentFocus)
            {
            case 0:
                if (m_currentOption < 1)
                {
                    m_currentOption++;
                    m_cursor->getComponent<TransformComponent>()->move(sf::Vector2f{0.0f, static_cast<float>(GLOBALS::SCALED_CUBE_SIZE)});
                }
                break;
            case 1:
                if (m_currentLevelFocus == 0)
                {
                    if (m_selectedLevel > 1)
                    {
                        m_selectedLevel--;
                        m_levelChange = true;
                    }
                }
                else if (m_currentLevelFocus == 1)
                {
                    if (m_selectedSublevel > 1)
                    {
                        m_selectedSublevel--;
                        m_levelChange = true;
                    }
                }
                break;
                default:
                    break;
            }
        }
        if (keyPressed->scancode == sf::Keyboard::Scancode::Left)
        {
            if (m_currentLevelFocus != 0)
            {
                m_currentLevelFocus = 0;
                m_underlineChange = true;
            }
        }

        if (keyPressed->scancode == sf::Keyboard::Scancode::Right)
        {
            if (m_currentLevelFocus != 1)
            {
                m_currentLevelFocus = 1;
                m_underlineChange = true;
            }
        }

    }
/*   Input& input = Input::Get();


   if (input.getKeyPressed(Key::MENU_LEFT)) {
      if (currentLevelFocus != 0) {
         currentLevelFocus = 0;
         underlineChange = true;
      }
   }

   if (input.getKeyPressed(Key::MENU_RIGHT)) {
      if (currentLevelFocus != 1) {
         currentLevelFocus = 1;
         underlineChange = true;
      }
   }
*/
}

void MenuSystem::handleInput()
{

}

void MenuSystem::onRemovedFromWorld(World *world)
{

}

int MenuSystem::getSelectedLevel()
{
    return m_selectedLevel;
}

int MenuSystem::getSelectedSublevel()
{
    return m_selectedSublevel;
}

void MenuSystem::enterLevelSelect()
{
    m_currentFocus = 1;

    m_levelSelectBackground->getComponent<TextureComponent>()->setVisible(true);
    m_levelSelectText->getComponent<TextComponent>()->setVisible(true);
    m_levelNumber->getComponent<TextComponent>()->setVisible(true);
    m_underline->getComponent<TextComponent>()->setVisible(true);

    m_cursor->getComponent<TextComponent>()->setVisible(false);
    m_selectText->getComponent<TextComponent>()->setVisible(false);
    m_optionsText->getComponent<TextComponent>()->setVisible(false);
}

void MenuSystem::exitLevelSelect()
{
    m_currentFocus = 0;

    m_levelSelectBackground->getComponent<TextureComponent>()->setVisible(false);
    m_levelSelectText->getComponent<TextComponent>()->setVisible(false);
    m_levelNumber->getComponent<TextComponent>()->setVisible(false);
    m_underline->getComponent<TextComponent>()->setVisible(false);

    m_cursor->getComponent<TextComponent>()->setVisible(true);
    m_selectText->getComponent<TextComponent>()->setVisible(true);
    m_optionsText->getComponent<TextComponent>()->setVisible(true);
}

void MenuSystem::hideMenuText()
{
    m_cursor->getComponent<TextComponent>()->setVisible(false);
    m_aboutText->getComponent<TextComponent>()->setVisible(false);
    m_selectText->getComponent<TextComponent>()->setVisible(false);
    m_optionsText->getComponent<TextComponent>()->setVisible(false);
    m_levelSelectText->getComponent<TextComponent>()->setVisible(false);
    m_levelNumber->getComponent<TextComponent>()->setVisible(false);
    m_underline->getComponent<TextComponent>()->setVisible(false);
}

void MenuSystem::showMenuText()
{
    m_cursor->getComponent<TextComponent>()->setVisible(true);
    m_aboutText->getComponent<TextComponent>()->setVisible(true);
    m_selectText->getComponent<TextComponent>()->setVisible(true);
    m_optionsText->getComponent<TextComponent>()->setVisible(true);
}
