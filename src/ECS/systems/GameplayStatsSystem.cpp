#include "GameplayStatsSystem.h"
#include "../../Globals.h"
#include "../Entity.h"
#include "../components/TransformComponent.h"
#include "../components/TextComponent.h"
#include "../components/TextureComponent.h"
#include "../components/AnimationComponent.h"
#include "../components/PausedAnimationComponent.h"
#include "../components/SpritesheetComponent.h"
#include "../components/IconComponent.h"
#include "../../utils/ResourceIdentifiers.h"
#include "../../Game.h"
#include "../../Map.h"
#include "../World.h"
#include "../../GameData.h"

GameplayStatsSystem::GameplayStatsSystem() {}

void GameplayStatsSystem::onAddedToWorld(World *world)
{
    int paddingW = 44;
    int paddingH = 16;
    int spacingH = 4;
    int textHeight = 16;

    int availableWidth = GLOBALS::WINDOW_WIDTH - paddingW;
    int columns = 4;
    float columnWidth = (float)availableWidth / (float)columns;

    m_marioText = world->create();
    m_marioText->addComponent<TransformComponent>(sf::Vector2f(0.0f, 0.0f));
    m_marioText->getComponent<TransformComponent>()->move(sf::Vector2f(paddingW, paddingH));
    m_marioText->addComponent<TextComponent>("MARIO", 16);

    m_scoreEntity = world->create();
    m_scoreEntity->addComponent<TransformComponent>(sf::Vector2f(0, 0));
    m_scoreEntity->getComponent<TransformComponent>()->move(
        sf::Vector2f(paddingW, paddingH + textHeight + spacingH));
    m_scoreEntity->addComponent<TextComponent>("000000", 16);

    m_coinIcon = world->create();
    m_coinIcon->addComponent<TransformComponent>(sf::Vector2f(0.0f, 0.0f));
    m_coinIcon->getComponent<TransformComponent>()->move(
        sf::Vector2f(paddingW + columnWidth, paddingH + textHeight + spacingH + 2));
    m_coinIcon->addComponent<TextureComponent>(
        Game::s_textureHolder->get(TextureID::BlockTileSheet));
    m_coinIcon->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                                   GLOBALS::ORIGINAL_CUBE_SIZE,
                                                   1,
                                                   1,
                                                   1,
                                                   GLOBALS::ORIGINAL_CUBE_SIZE,
                                                   GLOBALS::ORIGINAL_CUBE_SIZE,
                                                   Map::BlockIDCoordinates.at(754));
    m_coinIcon->addComponent<AnimationComponent>(std::vector<int>{754, 755, 756, 757},
                                                 8,
                                                 Map::BlockIDCoordinates);

    m_coinIcon->addComponent<PausedAnimationComponent>(0, 25);
    m_coinIcon->addComponent<IconComponent>();

    m_coinsEntity = world->create();

    m_coinsEntity->addComponent<TransformComponent>(sf::Vector2f(0.0f, 0.0f));
    m_coinsEntity->getComponent<TransformComponent>()->move(
        sf::Vector2f(paddingW + columnWidth + 18, paddingH + textHeight + spacingH));

    m_coinsEntity->addComponent<TextComponent>("x00", 16);

    m_worldEntity = world->create();
    m_worldEntity->addComponent<TransformComponent>(sf::Vector2f(0.0f, 0.0f));
    m_worldEntity->getComponent<TransformComponent>()->move(
        sf::Vector2f(paddingW + (2 * columnWidth), paddingH));
    m_worldEntity->addComponent<TextComponent>("WORLD", 16);

    m_worldNumberEntity = world->create();
    m_worldNumberEntity->addComponent<TransformComponent>(sf::Vector2f(0.0f, 0.0f));
    m_worldNumberEntity->getComponent<TransformComponent>()->move(
        sf::Vector2f(paddingW + (2 * columnWidth), paddingH + textHeight + spacingH));
    m_worldNumberEntity->addComponent<TextComponent>(std::to_string(GameData::LEVEL) + "-"
                                                         + std::to_string(GameData::SUB_LEVEL),
                                                     16);

    m_timeEntity = world->create();
    m_timeEntity->addComponent<TransformComponent>(sf::Vector2f(0.0f, 0.0f));
    m_timeEntity->getComponent<TransformComponent>()->move(
        sf::Vector2f(paddingW + (3 * columnWidth), paddingH));
    m_timeEntity->addComponent<TextComponent>("TIME", 16);

    m_timerEntity = world->create();
    m_timerEntity->addComponent<TransformComponent>(sf::Vector2f(0.0f, 0.0f));
    m_timerEntity->getComponent<TransformComponent>()->move(
        sf::Vector2f(paddingW + (3 * columnWidth) + 4, paddingH + textHeight + spacingH));
    m_timerEntity->addComponent<TextComponent>(std::to_string(GameData::GAME_TIME), 16);

    m_worldNumberTransition = world->create();
    m_worldNumberTransition->addComponent<TransformComponent>(sf::Vector2f(0.0f, 0.0f));
    m_worldNumberTransition->getComponent<TransformComponent>()->move(
        sf::Vector2f(paddingW + (columnWidth * 1.5), paddingH * 10 + textHeight + spacingH));
    m_worldNumberTransition->addComponent<TextComponent>("WORLD " + std::to_string(GameData::LEVEL)
                                                             + "-"
                                                             + std::to_string(GameData::SUB_LEVEL),
                                                         16);

    m_marioIcon = world->create();
    m_marioIcon->addComponent<TransformComponent>(sf::Vector2f{0.0f, 0.0f});
    m_marioIcon->getComponent<TransformComponent>()->move(
        sf::Vector2f{paddingW + (columnWidth * 1.5f), paddingH * 12.f + textHeight + spacingH});
    m_marioIcon->addComponent<TextureComponent>(
        Game::s_textureHolder->get(TextureID::PlayerSprteSheet));
    m_marioIcon->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                                    GLOBALS::ORIGINAL_CUBE_SIZE,
                                                    1,
                                                    9,
                                                    0,
                                                    GLOBALS::ORIGINAL_CUBE_SIZE,
                                                    GLOBALS::ORIGINAL_CUBE_SIZE,
                                                    Map::PlayerIDCoordinates.at(0));
    m_marioIcon->addComponent<IconComponent>();

    m_livesText = world->create();
    m_livesText->addComponent<TransformComponent>(sf::Vector2f{0.0f, 0.0f});
    m_livesText->getComponent<TransformComponent>()->move(
        sf::Vector2f{paddingW * 2 + (columnWidth * 1.5f),
                     paddingH * 12.0f + textHeight + spacingH * 3});
    m_livesText->addComponent<TextComponent>(" x  " + std::to_string(GameData::LIVES), 16);
}

void GameplayStatsSystem::tick(World *world)
{

}

void GameplayStatsSystem::handleInput(const std::optional<sf::Event> &event)
{

}

void GameplayStatsSystem::onRemovedFromWorld(World *world)
{

}
