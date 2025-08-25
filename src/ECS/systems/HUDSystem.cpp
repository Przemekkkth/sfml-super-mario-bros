#include "HUDSystem.h"
#include "../../GameData.h"
#include "../../Globals.h"
#include "../Entity.h"
#include "../World.h"
#include "../components/AddScoreComponent.h"
#include "../components/CreateFloatingTextComponent.h"
#include "../components/DestroyDelayedComponent.h"
#include "../components/FloatingTextComponent.h"
#include "../components/MovingComponent.h"
#include "../components/TextComponent.h"
#include "../components/TextureComponent.h"
#include "../components/TransformComponent.h"
#include <iomanip>
#include <sstream>

HUDSystem::HUDSystem() {}

void HUDSystem::onAddedToWorld(World *world)
{
    GameplayStatsSystem::onAddedToWorld(world);
    m_marioIcon->getComponent<TextureComponent>()->setVisible(false);
    m_worldNumberTransition->getComponent<TextComponent>()->setVisible(false);
    m_livesText->getComponent<TextComponent>()->setVisible(false);
    m_changeTime = false;
    m_time = 400 * GLOBALS::FPS;
    m_gameTime = 400;
}

void HUDSystem::tick(World *world)
{
    world->find<CreateFloatingTextComponent>([=](Entity *entity) {
        auto *floatingText = entity->getComponent<CreateFloatingTextComponent>();

        createFloatingText(world, floatingText->getOriginalEntity(), floatingText->getText());

        world->destroy(entity);
    });

    world->find<AddScoreComponent>([&](Entity *entity) {
        auto *score = entity->getComponent<AddScoreComponent>();

        if (score->getScore() > 0) {
            //totalScore += score->getScore();
            //changeScore = true;
            GameData::SCORE += score->getScore();

            std::ostringstream oss;

            oss << std::setw(6) << std::setfill('0') << GameData::SCORE;

            std::string result = oss.str();

            m_scoreEntity->getComponent<TextComponent>()->setText(result);
        }

        if (score->getAddCoinCount()) {
            //coins++;
            //schangeCoin = true;
            GameData::COINS++;
            m_coinsEntity->getComponent<TextComponent>()->setText(
                GameData::COINS < 10 ? "x0" + std::to_string(GameData::COINS)
                                     : "x" + std::to_string(GameData::COINS));
        }
        world->destroy(entity);
    });

    m_time -= 1;
    if ((m_time % GLOBALS::FPS) == 0 && shouldDecreaseGameTimeByTick) {
        m_gameTime -= 1;
        m_changeTime = true;
    }

    if (m_changeTime && shouldDecreaseGameTimeByTick) {
        m_timerEntity->getComponent<TextComponent>()->setText(std::to_string(m_gameTime));
        m_changeTime = false;
    }
}

int HUDSystem::getGameTime() const
{
    return m_gameTime;
}

void HUDSystem::scoreCountdown()
{
    m_gameTime--;
    m_timerEntity->getComponent<TextComponent>()->setText(std::to_string(m_gameTime));
    shouldDecreaseGameTimeByTick = false;
}

bool HUDSystem::scoreCountdownFinished() const
{
    return m_gameTime <= 0;
}

void HUDSystem::createFloatingText(World *world, Entity *originalEntity, std::string text)
{
    auto *originalPosition = originalEntity->getComponent<TransformComponent>();

    Entity *scoreText(world->create());
    scoreText->addComponent<TransformComponent>(
        sf::Vector2f(originalPosition->getCenterX(), originalPosition->getTop() - 4));
    scoreText->addComponent<MovingComponent>(sf::Vector2f(0.0f, -1.0f), sf::Vector2f(0.0f, 0.0f));
    scoreText->addComponent<TextComponent>(text, 10, true);
    scoreText->addComponent<FloatingTextComponent>();
    scoreText->addComponent<DestroyDelayedComponent>(35);
}
