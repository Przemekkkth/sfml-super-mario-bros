#include "MapSystem.h"
#include "../../AABB.h"
#include "../../Camera.h"
#include "../../Game.h"
#include "../../GameData.h"
#include "../../commands/CommandScheduler.h"
#include "../../commands/RunCommand.h"
#include "../../commands/SequenceCommand.h"
#include "../../commands/WaitCommand.h"
#include "../../utils/ResourceIdentifiers.h"
#include "../Entity.h"
#include "../World.h"
#include "../components/AboveForegroundComponent.h"
#include "../components/AddScoreComponent.h"
#include "../components/AnimationComponent.h"
#include "../components/AxeComponent.h"
#include "../components/BackgroundComponent.h"
#include "../components/BottomCollisionComponent.h"
#include "../components/BowserComponent.h"
#include "../components/BridgeChainComponent.h"
#include "../components/BridgeComponent.h"
#include "../components/BumpableComponent.h"
#include "../components/CallbackComponent.h"
#include "../components/CollectibleComponent.h"
#include "../components/CollisionExemptComponent.h"
#include "../components/CreateFloatingTextComponent.h"
#include "../components/CrushableComponent.h"
#include "../components/DeadComponent.h"
#include "../components/DestroyDelayedComponent.h"
#include "../components/DestroyOutsideCameraComponent.h"
#include "../components/DestructibleComponent.h"
#include "../components/EnemyComponent.h"
#include "../components/FireBarComponent.h"
#include "../components/FlagComponent.h"
#include "../components/FlagPoleComponent.h"
#include "../components/FloatingTextComponent.h"
#include "../components/ForegroundComponent.h"
#include "../components/FrictionExemptComponent.h"
#include "../components/FrozenComponent.h"
#include "../components/GravityComponent.h"
#include "../components/HammerBroComponent.h"
#include "../components/InvisibleBlockComponent.h"
#include "../components/LakituComponent.h"
#include "../components/MoveOutsideCameraComponent.h"
#include "../components/MovingComponent.h"
#include "../components/MovingPlatformComponent.h"
#include "../components/MysteryBoxComponent.h"
#include "../components/ParticleComponent.h"
#include "../components/PausedAnimationComponent.h"
#include "../components/PiranhaPlantComponent.h"
#include "../components/PlatformLevelComponent.h"
#include "../components/PlayerComponent.h"
#include "../components/ProjectileComponent.h"
#include "../components/SoundComponent.h"
#include "../components/SpritesheetComponent.h"
#include "../components/TextComponent.h"
#include "../components/TextureComponent.h"
#include "../components/TileComponent.h"
#include "../components/TimerComponent.h"
#include "../components/TopCollisionComponent.h"
#include "../components/TrampolineComponent.h"
#include "../components/TransformComponent.h"
#include "../components/VineComponent.h"
#include "../components/WaitUntilComponent.h"
#include "../components/WarpPipeComponent.h"
#include <iostream>
#include <random>

int MapSystem::INVALID_CODE = -1;

MapSystem::MapSystem() {}

void MapSystem::onAddedToWorld(World *world)
{
    m_world = world;
    loadLevel();
    loadEntities();
}

void MapSystem::tick(World *world) {}

LevelData &MapSystem::getLevelData()
{
    return m_levelData;
}

void MapSystem::loadLevel()
{
    auto folderPath = "assets/data/World" + std::to_string(GameData::LEVEL) + "-"
                      + std::to_string(GameData::SUB_LEVEL) + "/";
    auto mapDataPath = folderPath + "World" + std::to_string(GameData::LEVEL) + "-"
                       + std::to_string(GameData::SUB_LEVEL);

    auto backgroundPath = mapDataPath + "_Background.csv";
    auto undergroundPath = mapDataPath + "_Underground.csv";
    auto foregroundPath = mapDataPath + "_Foreground.csv";
    auto collectiblePath = mapDataPath + "_Collectibles.csv";
    auto enemyPath = mapDataPath + "_Enemies.csv";
    auto aboveForegroundPath = mapDataPath + "_Above_Foreground.csv";

    m_backgroundMap.loadMap(backgroundPath.c_str());
    m_undergroundMap.loadMap(undergroundPath.c_str());
    m_foregroundMap.loadMap(foregroundPath.c_str());
    m_collectiblesMap.loadMap(collectiblePath.c_str());
    m_enemyMap.loadMap(enemyPath.c_str());
    m_aboveForegroundMap.loadMap(aboveForegroundPath.c_str());
}

void MapSystem::loadEntities()
{
    createBackgroundEntities();
    createUndergroundEntities();
    createPlatformLevelEntities();
    createForegroundEntities();
    createFireBarEntities();
    createEnemyEntities();
    createAboveForegroundEntities();
    createFloatingTextEntities();
}

void MapSystem::createBackgroundEntities()
{
    auto mapHeight = m_backgroundMap.getLevelData().size();
    auto mapWidth = m_backgroundMap.getLevelData()[0].size();

    for (size_t y = 0; y < mapHeight; ++y) {
        for (size_t x = 0; x < mapWidth; ++x) {
            auto entityID = m_backgroundMap.getLevelData()[y][x];
            auto referenceID = getReferenceBlockID(entityID);
            if (referenceID != MapSystem::INVALID_CODE && referenceID != 391 && referenceID != 393) {
                createBackgroundEntity(x, y, entityID);
            }
        }
    }
}

void MapSystem::createUndergroundEntities()
{
    auto mapHeight = m_undergroundMap.getLevelData().size();
    auto mapWidth = m_undergroundMap.getLevelData()[0].size();

    for (size_t y = 0; y < mapHeight; ++y) {
        for (size_t x = 0; x < mapWidth; ++x) {
            auto entityID = m_undergroundMap.getLevelData()[y][x];
            auto referenceID = getReferenceBlockID(entityID);
            if (m_levelData.getLevelType() == LevelType::StartUnderground) {
                createInvsibleBlock(x, y, referenceID);
            }
            createForegroundEntity(x, y, entityID, referenceID);
        }
    }
}

void MapSystem::createPlatformLevelEntities()
{
    for (int i = 0; i < getLevelData().getPlatformLevels().size(); ++i) {
        auto platformLevelData = getLevelData().getPlatformLevels().at(i);
        createPlatformLevel(platformLevelData);
    }
}

void MapSystem::createForegroundEntities()
{
    auto mapHeight = m_foregroundMap.getLevelData().size();
    auto mapWidth = m_foregroundMap.getLevelData()[0].size();
    for (size_t y = 0; y < mapHeight; ++y) {
        for (size_t x = 0; x < mapWidth; ++x) {
            auto entityID = m_foregroundMap.getLevelData()[y][x];
            auto referenceID = getReferenceBlockID(entityID);
            if (m_levelData.getLevelType() != LevelType::StartUnderground) {
                createInvsibleBlock(x, y, referenceID);
            }
            createForegroundEntity(x, y, entityID, referenceID);
        }
    }
}

void MapSystem::createFireBarEntities()
{
    for (int i = 0; i < getLevelData().getFireBars().size(); ++i) {
        auto fireBar = getLevelData().getFireBars()[i];
        for (int j = 0; j < fireBar.length; ++j) {
            auto barElement = m_world->create();

            barElement->addComponent<TransformComponent>(
                sf::Vector2f(fireBar.coords.x * GLOBALS::SCALED_CUBE_SIZE,
                             fireBar.coords.y * GLOBALS::SCALED_CUBE_SIZE),
                sf::Vector2i(1, 1),
                sf::Rect<int>({0, 0},
                              {GLOBALS::SCALED_CUBE_SIZE / 4, GLOBALS::SCALED_CUBE_SIZE / 4}));
            barElement->addComponent<TextureComponent>(
                Game::s_textureHolder->get(TextureID::BlockTileSheet));
            barElement->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                                           GLOBALS::ORIGINAL_CUBE_SIZE,
                                                           1,
                                                           1,
                                                           1,
                                                           GLOBALS::ORIGINAL_CUBE_SIZE,
                                                           GLOBALS::ORIGINAL_CUBE_SIZE,
                                                           Map::BlockIDCoordinates.at(611));
            std::vector<int> frameIDs{611, 612, 613, 614};
            int framesPerSec = 12;
            barElement->addComponent<AnimationComponent>(frameIDs,
                                                         framesPerSec,
                                                         Map::BlockIDCoordinates);
            barElement->addComponent<FireBarComponent>(sf::Vector2f(fireBar.coords.x
                                                                        * GLOBALS::SCALED_CUBE_SIZE,
                                                                    fireBar.coords.y
                                                                        * GLOBALS::SCALED_CUBE_SIZE),
                                                       j * GLOBALS::ORIGINAL_CUBE_SIZE,
                                                       fireBar.angle,
                                                       fireBar.direction);
            barElement->addComponent<TimerComponent>(
                [=](Entity *entity) {
                    auto barComponent = entity->getComponent<FireBarComponent>();
                    auto type = barComponent->getDirection();
                    if (type == RotationDirection::Clockwise) {
                        barComponent->getBarAngle() -= 10;
                    } else if (type == RotationDirection::CounterClockwise) {
                        barComponent->getBarAngle() += 10;
                    }
                },
                6);
            if (j != (fireBar.length - 1)) {
                barElement->addComponent<EnemyComponent>(EnemyType::FireBar);
            }
            barElement->addComponent<ForegroundComponent>();
        }
    }
}

void MapSystem::createEnemyEntities()
{
    auto mapHeight = m_enemyMap.getLevelData().size();
    auto mapWidth = m_enemyMap.getLevelData()[0].size();

    for (size_t y = 0; y < mapHeight; ++y) {
        for (size_t x = 0; x < mapWidth; ++x) {
            auto entityID = m_enemyMap.getLevelData()[y][x];
            auto referenceID = getReferenceEnemyID(entityID);
            if (entityID != -1 || entityID != 73 || entityID != 83 || entityID != 85
                || entityID != 91 || entityID != 91 || entityID != 490 || entityID != 492
                || entityID != 496) {
                createEnemyEntity(x, y, entityID, referenceID);
            }
        }
    }
}

void MapSystem::createAboveForegroundEntities()
{
    const int WARP_PIPE_CODE1 = 150;
    const int WARP_PIPE_CODE2 = 292;
    const int WARP_PIPE_CODE3 = 151;
    auto mapHeight = m_aboveForegroundMap.getLevelData().size();
    auto mapWidth = m_aboveForegroundMap.getLevelData()[0].size();
    //return;
    for (size_t y = 0; y < mapHeight; ++y) {
        for (size_t x = 0; x < mapWidth; ++x) {
            auto entityID = m_aboveForegroundMap.getLevelData()[y][x];
            auto referenceID = getReferenceBlockID(entityID);
            if (referenceID == WARP_PIPE_CODE1 || referenceID == WARP_PIPE_CODE2
                || referenceID == WARP_PIPE_CODE3) {
                auto entity = m_world->create();
                entity->addComponent<TransformComponent>(
                    sf::Vector2f(x * GLOBALS::SCALED_CUBE_SIZE, y * GLOBALS::SCALED_CUBE_SIZE));
                entity->addComponent<TextureComponent>(
                    Game::s_textureHolder->get(TextureID::BlockTileSheet));
                entity->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                                           GLOBALS::ORIGINAL_CUBE_SIZE,
                                                           1,
                                                           1,
                                                           1,
                                                           GLOBALS::ORIGINAL_CUBE_SIZE,
                                                           GLOBALS::ORIGINAL_CUBE_SIZE,
                                                           Map::BlockIDCoordinates.at(entityID));
                entity->addComponent<AboveForegroundComponent>();
                auto warpPipeLocaions = getLevelData().getWarpPipes();
                for (size_t i = 0; i < warpPipeLocaions.size(); ++i) {
                    if (x == warpPipeLocaions.at(i).pipeCoords.x
                        && y == warpPipeLocaions.at(i).pipeCoords.y) {
                        if (warpPipeLocaions.at(i).goingIn != Direction::None) {
                            addWarpPipeComponetToEntity(entity, warpPipeLocaions.at(i));
                        }
                    }
                }
            } else if (referenceID != MapSystem::INVALID_CODE) {
                createAboveForegroundEntity(x, y, entityID);
            }
        }
    }
}

void MapSystem::createFloatingTextEntities()
{
    for (int i = 0; i < getLevelData().getFloatingTexts().size(); ++i) {
        auto text = m_world->create();
        auto floatingTextData = getLevelData().getFloatingTexts()[i];
        text->addComponent<TransformComponent>(
            sf::Vector2f(floatingTextData.pos.x * GLOBALS::SCALED_CUBE_SIZE,
                         floatingTextData.pos.y * GLOBALS::SCALED_CUBE_SIZE));
        text->addComponent<TextComponent>(floatingTextData.text, 16, true);
        text->addComponent<FloatingTextComponent>();
    }
}

Entity *MapSystem::createBackgroundEntity(int x, int y, int id)
{
    Entity *entity = m_world->create();
    entity->addComponent<TransformComponent>(
        sf::Vector2f(x * GLOBALS::SCALED_CUBE_SIZE, y * GLOBALS::SCALED_CUBE_SIZE));
    entity->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::BlockTileSheet));
    entity->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               1,
                                               1,
                                               1,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               Map::BlockIDCoordinates[id]);
    entity->addComponent<BackgroundComponent>();
    return entity;
}

void MapSystem::createForegroundEntity(int x, int y, int id, int referenceID)
{
    const int BULLET_BILL_CANNON_CODE = 63;
    const int FLAG_POLE_CODE1 = 101;
    const int COIN_CODE1 = 144;
    const int FLAG_POLE_CODE2 = 149;
    const int FLAG_CODE = 152;
    const int COIN_CODE2 = 176;
    const int QUESTION_BLOCK_CODE = 192;
    const int AXE_CODE = 240;
    const int BRICK_CODE1 = 289;
    const int BRICK_CODE2 = 290;
    const int BRIDGE_CHAIN_CODE = 339;
    const int TRAMPOLINE_CODE = 346;
    const int BRIDGE_CODE = 392;
    const int MOVING_PLATFORM_1WIDE_CODE = 609;
    const int MOVING_PLATFORM_2WIDE_CODE = 761;
    const int MOVING_PLATFORM_3WIDE_CODE = 809;
    const int CLOUD_PLATFORM_CODE1 = 610;
    const int CLOUD_PLATFORM_CODE2 = 857;

    if (referenceID == COIN_CODE1 || referenceID == COIN_CODE2) {
        createCoin(x, y, referenceID);
    } else if (referenceID == BULLET_BILL_CANNON_CODE) {
        createBulletBillCannon(x, y, referenceID);
    } else if (referenceID == FLAG_POLE_CODE1 || referenceID == FLAG_POLE_CODE2) {
        createFlagPole(x, y, referenceID);
    } else if (referenceID == FLAG_CODE) {
        createFlag(x, y, referenceID);
    } else if (referenceID == QUESTION_BLOCK_CODE) {
        createQuestionBlock(x, y, referenceID);
    } else if (referenceID == AXE_CODE) {
        createAxe(x, y, id);
    } else if (referenceID == BRICK_CODE1 || referenceID == BRICK_CODE2) {
        Entity *brickEntity = createBlockEntity(x, y, id);
        int debrisId = getReferenceBlockIDAsEntity(id, 291);
        brickEntity->addComponent<DestructibleComponent>(Map::BlockIDCoordinates.at(debrisId));
        brickEntity->addComponent<BumpableComponent>();
        MysteryBoxType boxType = MysteryBoxType::None;
        int collectibleID = m_collectiblesMap.getLevelData()[y][x];
        if (collectibleID != MapSystem::INVALID_CODE) {
            int referenceCollectibleID = getReferenceBlockID(collectibleID);
            const int OneUpCode = 52;
            const int SuperStarCode = 96;
            const int CoinCode = 144;
            const int VinesCode = 148;
            const int MushroomCode = 608;
            switch (referenceCollectibleID) {
            case OneUpCode:
                boxType = MysteryBoxType::OneUp;
                break;
            case SuperStarCode:
                boxType = MysteryBoxType::SuperStar;
                break;
            case CoinCode:
                boxType = MysteryBoxType::Coins;
                break;
            case VinesCode:
                boxType = MysteryBoxType::Vines;
                break;
            case MushroomCode:
                boxType = MysteryBoxType::Mushroom;
                break;
            default:
                break;
            }
        }

        if (boxType != MysteryBoxType::None) {
            brickEntity->addComponent<MysteryBoxComponent>(boxType);
            addItemDispenser(brickEntity, id);
        }

    } else if (referenceID == BRIDGE_CHAIN_CODE) {
        createBridgeChain(x, y, id);
    } else if (referenceID == TRAMPOLINE_CODE) {
        Entity *trampolineTop(createBlockEntity(x, y, id));
        Entity *trampolineBottom(createBlockEntity(x, y + 1, id + 48));
        trampolineBottom->remove<TileComponent>();
        int topIDs[3]{id, id + 1, id + 2};
        int bottomIDs[3]{id + 48, id + 1 + 48, id + 2 + 48};

        trampolineTop->addComponent<TrampolineComponent>(trampolineBottom, topIDs, bottomIDs);
    } else if (referenceID == BRIDGE_CODE) {
        createBridge(x, y, id);
    } else if (referenceID == MOVING_PLATFORM_1WIDE_CODE) {
        createBlockEntity(x, y, id);
    } else if (referenceID == MOVING_PLATFORM_2WIDE_CODE) {
        create2WideMovingPlatform(x, y, id);
    } else if (referenceID == MOVING_PLATFORM_3WIDE_CODE) {
        create3WideMovingPlatform(x, y, id);
    } else if (referenceID == CLOUD_PLATFORM_CODE1 || referenceID == CLOUD_PLATFORM_CODE2) {
        createCloudPlatform(x, y, CLOUD_PLATFORM_CODE2);
    } else if (referenceID != MapSystem::INVALID_CODE && referenceID != 762 && referenceID != 810
               && referenceID != 811 && referenceID != 858 && referenceID != 859) {
        createBlockEntity(x, y, id);
    }
}

void MapSystem::createAboveForegroundEntity(int x, int y, int id)
{
    auto entity = m_world->create();
    entity->addComponent<TransformComponent>(
        sf::Vector2f(x * GLOBALS::SCALED_CUBE_SIZE, y * GLOBALS::SCALED_CUBE_SIZE));
    entity->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::BlockTileSheet));
    entity->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               1,
                                               1,
                                               1,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               Map::BlockIDCoordinates.at(id));
    entity->addComponent<AboveForegroundComponent>();
}

void MapSystem::createInvsibleBlock(int x, int y, int referenceID)
{
    auto collectibleID = m_collectiblesMap.getLevelData()[y][x];
    if (referenceID == MapSystem::INVALID_CODE && collectibleID != MapSystem::INVALID_CODE) {
        auto entity = createBlockEntity(x, y, 53);
        entity->addComponent<InvisibleBlockComponent>();
        entity->addComponent<BumpableComponent>();
        auto collectibleType = MysteryBoxType::None;
        auto referenceCollectibleID = getReferenceBlockID(collectibleID);

        int blankBlockID = MapSystem::INVALID_CODE;
        if (collectibleID != 608) {
            blankBlockID = getReferenceBlockIDAsEntity(collectibleID, 53);
        } else {
            auto levelType = m_levelData.getLevelType();
            if (levelType == LevelType::Underground || levelType == LevelType::StartUnderground) {
                blankBlockID = 69;
            } else if (levelType == LevelType::Castle) {
                blankBlockID = 581;
            } else {
                blankBlockID = 53;
            }
        }

        if (referenceCollectibleID == 52) {
            collectibleType = MysteryBoxType::OneUp;
        } else if (referenceCollectibleID == 96) {
            collectibleType = MysteryBoxType::SuperStar;
        } else if (referenceCollectibleID == 144) {
            collectibleType = MysteryBoxType::Coins;
        } else if (referenceCollectibleID == 608) {
            collectibleType = MysteryBoxType::Mushroom;
        }

        if (collectibleType != MysteryBoxType::None) {
            entity->addComponent<MysteryBoxComponent>(collectibleType);
            addItemDispenser(entity, blankBlockID);
        }
    }
}

void MapSystem::createQuestionBlock(int x, int y, int referenceID)
{
    Entity *entity = createBlockEntity(x, y, referenceID);
    entity->addComponent<AnimationComponent>(std::vector<int>{referenceID,
                                                              referenceID + 1,
                                                              referenceID + 2,
                                                              referenceID + 3},
                                             8,
                                             Map::BlockIDCoordinates);
    entity->addComponent<PausedAnimationComponent>(0, 25);
    entity->addComponent<BumpableComponent>();
    auto collectibleType = MysteryBoxType::Coins;
    auto collectibleID = m_collectiblesMap.getLevelData()[y][x];
    if (collectibleID != MapSystem::INVALID_CODE) {
        if (collectibleID == 52) {
            collectibleType = MysteryBoxType::OneUp;
        } else if (collectibleID == 96) {
            collectibleType = MysteryBoxType::SuperStar;
        } else if (collectibleID == 144) {
            collectibleType = MysteryBoxType::Coins;
        } else if (collectibleID == 608) {
            collectibleType = MysteryBoxType::Mushroom;
        }
    }

    if (collectibleType != MysteryBoxType::None) {
        entity->addComponent<MysteryBoxComponent>(collectibleType);
        addItemDispenser(entity, referenceID);
    }
}

void MapSystem::createAxe(int x, int y, int id)
{
    auto entity = m_world->create();
    entity->addComponent<TransformComponent>(
        sf::Vector2f(x * GLOBALS::SCALED_CUBE_SIZE, y * GLOBALS::SCALED_CUBE_SIZE));
    entity->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::BlockTileSheet));
    entity->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               1,
                                               1,
                                               1,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               Map::BlockIDCoordinates.at(id));
    std::vector<int> frameIDs{id, id + 1, id + 2, id + 3};
    const int framesPerSecond = 8;
    entity->addComponent<AnimationComponent>(frameIDs, framesPerSecond, Map::BlockIDCoordinates);
    entity->addComponent<ForegroundComponent>();
    entity->addComponent<PausedAnimationComponent>(1, 25);
    entity->addComponent<AxeComponent>();
}

void MapSystem::createBridgeChain(int x, int y, int id)
{
    auto isCastleLevelType = getLevelData().getLevelType() == LevelType::Castle;
    if (isCastleLevelType) {
        auto bridgeChain = createBlockEntity(x, y, id);
        bridgeChain->addComponent<BridgeChainComponent>();
    } else {
        createBlockEntity(x, y, id);
    }
}

void MapSystem::createCloudPlatform(int x, int y, int id)
{
    auto entity = m_world->create();
    entity->addComponent<TransformComponent>(sf::Vector2f(x * GLOBALS::SCALED_CUBE_SIZE,
                                                          y * GLOBALS::SCALED_CUBE_SIZE),
                                             sf::Vector2i(3, 1),
                                             sf::Rect<int>({0, 0},
                                                           {3 * GLOBALS::SCALED_CUBE_SIZE,
                                                            GLOBALS::SCALED_CUBE_SIZE}));
    entity->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::BlockTileSheet));
    entity->addComponent<SpritesheetComponent>(3 * GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               1,
                                               1,
                                               1,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               Map::BlockIDCoordinates.at(id));
    entity->addComponent<MovingComponent>(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(0.0f, 0.0f));
    entity->addComponent<FrictionExemptComponent>();
    entity->addComponent<WaitUntilComponent>(
        [](Entity *entity) { return entity->hasComponent<TopCollisionComponent>(); },
        [](Entity *entity) {
            entity->getComponent<MovingComponent>()->getVelocity().x = 2.0f;
            entity->remove<WaitUntilComponent>();
        });
    entity->addComponent<ForegroundComponent>();
    entity->addComponent<TileComponent>();
}

void MapSystem::createBridge(int x, int y, int id)
{
    const int BRIDGE_CODE = 392;
    auto isCastleLevelType = getLevelData().getLevelType() == LevelType::Castle;
    if (isCastleLevelType) {
        if (getReferenceBlockID(m_foregroundMap.getLevelData()[y][x - 1]) != BRIDGE_CODE) {
            auto bridge = createBlockEntity(x, y, id);
            auto bridgeComponent = bridge->addComponent<BridgeComponent>();
            bridgeComponent->getConnectedBridgeParts().push_back(bridge);
            auto futureCoordinateCheck = x + 1;
            while (getReferenceBlockID(m_foregroundMap.getLevelData()[y][futureCoordinateCheck])
                   == BRIDGE_CODE) {
                auto connectedBridge = createBlockEntity(futureCoordinateCheck, y, id);
                bridgeComponent->getConnectedBridgeParts().push_back(connectedBridge);
                futureCoordinateCheck++;
            }
        }
    } else {
        createBlockEntity(x, y, id);
    }
}

int MapSystem::getReferenceBlockID(int entityID)
{
    if (entityID == MapSystem::INVALID_CODE) {
        return MapSystem::INVALID_CODE;
    }

    auto irregularBlockReferences = Map::IrregularBlockReferences;
    if (irregularBlockReferences.find(entityID) != irregularBlockReferences.end()) {
        return irregularBlockReferences[entityID];
    }

    auto blockIDCoordinate = Map::BlockIDCoordinates[entityID];
    int coordinateX = blockIDCoordinate.x;
    int coordinateY = blockIDCoordinate.y;
    const int mapWorldWidthInTiles = 16;
    const int mapWorldHeightInTiles = 10;

    if (coordinateY > mapWorldHeightInTiles && blockIDCoordinate.x < 2 * mapWorldWidthInTiles) {
        coordinateY -= mapWorldHeightInTiles + 1;
    }

    if (coordinateX >= mapWorldWidthInTiles && coordinateX < 2 * mapWorldWidthInTiles) {
        coordinateX -= mapWorldWidthInTiles;
    } else if (coordinateX > 2 * mapWorldWidthInTiles
               && blockIDCoordinate.y < mapWorldHeightInTiles) {
        coordinateX -= 2 * mapWorldWidthInTiles;
    }

    for (const auto &[key, blockCoord] : Map::BlockIDCoordinates) {
        if (blockCoord.x == coordinateX && blockCoord.y == coordinateY) {
            return key;
        }
    }

    return MapSystem::INVALID_CODE;
}

int MapSystem::getReferenceBlockIDAsEntity(int entityID, int referenceID)
{
    if (entityID == MapSystem::INVALID_CODE) {
        return MapSystem::INVALID_CODE;
    }

    auto entityCoordinates = Map::BlockIDCoordinates.at(entityID);
    auto referenceCoordinates = Map::BlockIDCoordinates.at(referenceID);

    auto entityCoordinateX = entityCoordinates.x;
    auto entityCoordinateY = entityCoordinates.y;

    auto referenceCoordinateX = referenceCoordinates.x;
    auto referenceCoordinateY = referenceCoordinates.y;

    auto mapWorldWidthInTiles = 16;
    auto mapWorldHeightInTiles = 10;

    if (entityCoordinateY > mapWorldHeightInTiles) {
        referenceCoordinateY += mapWorldHeightInTiles + 1;
    }

    if (entityCoordinateX > mapWorldHeightInTiles - 1
        && entityCoordinateX < 2 * mapWorldWidthInTiles) {
        referenceCoordinateX += mapWorldWidthInTiles;
    } else if (entityCoordinateX >= 2 * mapWorldWidthInTiles) {
        referenceCoordinateX -= 2 * mapWorldWidthInTiles;
    }

    for (auto blockIDCoordinate = Map::BlockIDCoordinates.begin();
         blockIDCoordinate != Map::BlockIDCoordinates.end();
         ++blockIDCoordinate) {
        if (blockIDCoordinate->second.x == referenceCoordinateX
            && blockIDCoordinate->second.y == referenceCoordinateY) {
            return blockIDCoordinate->first;
        }
    }

    return MapSystem::INVALID_CODE;
}

int MapSystem::getReferenceEnemyIDAsEntity(int entityID, int referenceID)
{
    if (entityID == MapSystem::INVALID_CODE || referenceID == MapSystem::INVALID_CODE) {
        return -1;
    }

    auto entityCoordinates = Map::EnemyIDCoordinates.at(entityID);
    auto referenceCoordinates = Map::EnemyIDCoordinates.at(referenceID);

    auto entityCoordinateY = entityCoordinates.y;
    auto referenceCoordinateX = referenceCoordinates.x;
    auto referenceCoordinateY = referenceCoordinates.y;

    if (entityCoordinateY && entityCoordinateY < 12) {
        referenceCoordinateY = referenceCoordinateY + (entityCoordinateY - (entityCoordinateY % 3));
    }

    for (auto enemyIDCoordinate = Map::EnemyIDCoordinates.begin();
         enemyIDCoordinate != Map::EnemyIDCoordinates.end();
         ++enemyIDCoordinate) {
        if (enemyIDCoordinate->second.x == referenceCoordinateX
            && enemyIDCoordinate->second.y == referenceCoordinateY) {
            return enemyIDCoordinate->first;
        }
    }

    return MapSystem::INVALID_CODE;
}

int MapSystem::getReferenceEnemyID(int id)
{
    if (id == MapSystem::INVALID_CODE) {
        return MapSystem::INVALID_CODE;
    }

    auto entityCoordinates = Map::EnemyIDCoordinates.at(id);
    auto coordinateX = entityCoordinates.x;
    auto coordinateY = entityCoordinates.y;

    if (coordinateY > 2 && coordinateY < 12) {
        coordinateY -= (coordinateY - (coordinateY % 3));
    }

    for (auto enemyIDCoordinate = Map::EnemyIDCoordinates.begin();
         enemyIDCoordinate != Map::EnemyIDCoordinates.end();
         ++enemyIDCoordinate) {
        if (enemyIDCoordinate->second.x == coordinateX
            && enemyIDCoordinate->second.y == coordinateY) {
            return enemyIDCoordinate->first;
        }
    }

    return MapSystem::INVALID_CODE;
}

void MapSystem::createCoin(int x, int y, int id)
{
    auto entity = m_world->create();
    entity->addComponent<TransformComponent>(
        sf::Vector2f(x * GLOBALS::SCALED_CUBE_SIZE, y * GLOBALS::SCALED_CUBE_SIZE));
    entity->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::BlockTileSheet));
    entity->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               1,
                                               1,
                                               1,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               Map::BlockIDCoordinates.at(id));
    entity->addComponent<AnimationComponent>(std::vector<int>{id, id + 1, id + 2, id + 3},
                                             8,
                                             Map::BlockIDCoordinates);
    entity->addComponent<PausedAnimationComponent>(0, 25);
    entity->addComponent<CollectibleComponent>(CollectibleType::Coin);
}

void MapSystem::createFlagPole(int x, int y, int id)
{
    auto entity = m_world->create();
    entity->addComponent<TransformComponent>(
        sf::Vector2f(x * GLOBALS::SCALED_CUBE_SIZE, y * GLOBALS::SCALED_CUBE_SIZE));
    entity->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::BlockTileSheet));
    entity->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               1,
                                               1,
                                               1,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               Map::BlockIDCoordinates.at(id));
    entity->addComponent<CollectibleComponent>(CollectibleType::Coin);
    entity->addComponent<ForegroundComponent>();
    entity->addComponent<FlagPoleComponent>();
}

void MapSystem::createFlag(int x, int y, int id)
{
    auto entity = m_world->create();
    entity->addComponent<TransformComponent>(
        sf::Vector2f(x * GLOBALS::SCALED_CUBE_SIZE + GLOBALS::SCALED_CUBE_SIZE / 2.0f,
                     y * GLOBALS::SCALED_CUBE_SIZE));
    entity->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::BlockTileSheet));
    entity->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               1,
                                               1,
                                               1,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               Map::BlockIDCoordinates.at(id));
    entity->addComponent<MovingComponent>(sf::Vector2f{0.0f, 0.0f}, sf::Vector2f{0.0f, 0.0f});
    entity->addComponent<ForegroundComponent>();
    entity->addComponent<FlagComponent>();
}

void MapSystem::createPlatformEntity(
    int x, int y, int id, int length, LevelData::MovingPlatform movingPlatform)
{
    auto platform = m_world->create();
    platform->addComponent<TransformComponent>(sf::Vector2f(x * GLOBALS::SCALED_CUBE_SIZE,
                                                            y * GLOBALS::SCALED_CUBE_SIZE),
                                               sf::Vector2i(length, 1));
    platform->getComponent<TransformComponent>()->getHitbox().size.x = length
                                                                       * GLOBALS::SCALED_CUBE_SIZE;
    platform->getComponent<TransformComponent>()->getHitbox().position.y = GLOBALS::SCALED_CUBE_SIZE;
    platform->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::BlockTileSheet));
    platform->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE * length,
                                                 GLOBALS::ORIGINAL_CUBE_SIZE,
                                                 1,
                                                 1,
                                                 1,
                                                 GLOBALS::ORIGINAL_CUBE_SIZE,
                                                 GLOBALS::ORIGINAL_CUBE_SIZE,
                                                 Map::BlockIDCoordinates.at(id));
    platform->addComponent<ForegroundComponent>();
    platform->addComponent<TileComponent>();

    if (movingPlatform.shift) {
        platform->getComponent<TransformComponent>()->setLeft(
            platform->getComponent<TransformComponent>()->getPosition().x
            + GLOBALS::SCALED_CUBE_SIZE / 2.0f);
    }

    platform->addComponent<FrictionExemptComponent>();
    platform->addComponent<CollisionExemptComponent>();
    platform->addComponent<MovingComponent>(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(0.0f, 0.0f));
    auto move = platform->getComponent<MovingComponent>();

    switch (movingPlatform.motion) {
    case PlatformMotionType::OneDirectionRepeated: {
        auto minX = movingPlatform.point.x * GLOBALS::SCALED_CUBE_SIZE;
        auto maxX = movingPlatform.point.y * GLOBALS::SCALED_CUBE_SIZE;
        platform->addComponent<MovingPlatformComponent>(PlatformMotionType::OneDirectionRepeated,
                                                        movingPlatform.direction,
                                                        sf::Vector2f(minX, maxX));
        if (movingPlatform.direction == Direction::Up) {
            move->getVelocity().y = -1.0f;
        } else if (movingPlatform.direction == Direction::Right) {
            move->getVelocity().x = 2.0f;
        } else if (movingPlatform.direction == Direction::Down) {
            move->getVelocity().y = 1.0f;
        } else if (movingPlatform.direction == Direction::Left) {
            move->getVelocity().x = -2.0f;
        }

    } break;
    case PlatformMotionType::OneDirectionContinuous: {
        platform->addComponent<MovingPlatformComponent>(PlatformMotionType::OneDirectionContinuous,
                                                        movingPlatform.direction,
                                                        sf::Vector2f(0.0f, 0.0f));
        if (movingPlatform.direction == Direction::Up) {
            move->getVelocity().y = -1.0f;
        } else if (movingPlatform.direction == Direction::Right) {
            move->getVelocity().x = 2.0f;
        } else if (movingPlatform.direction == Direction::Down) {
            move->getVelocity().y = 1.0f;
        } else if (movingPlatform.direction == Direction::Left) {
            move->getVelocity().x = -2.0f;
        }
    }
    case PlatformMotionType::BackAndForth: {
        auto minX = movingPlatform.point.x * GLOBALS::SCALED_CUBE_SIZE;
        auto maxX = movingPlatform.point.y * GLOBALS::SCALED_CUBE_SIZE;
        platform->addComponent<MovingPlatformComponent>(PlatformMotionType::BackAndForth,
                                                        movingPlatform.direction,
                                                        sf::Vector2f(minX, maxX));
        if (movingPlatform.direction == Direction::Up) {
            move->getVelocity().y = -1.0f;
        } else if (movingPlatform.direction == Direction::Right) {
            move->getVelocity().x = 2.0f;
        } else if (movingPlatform.direction == Direction::Down) {
            move->getVelocity().y = 1.0f;
        } else if (movingPlatform.direction == Direction::Left) {
            move->getVelocity().x = -2.0f;
        }
    } break;
    case PlatformMotionType::Gravity: {
        platform->addComponent<MovingPlatformComponent>(PlatformMotionType::Gravity,
                                                        Direction::None,
                                                        sf::Vector2f(0, 0));
    }
    default:
        break;
    }
}

void MapSystem::create2WideMovingPlatform(int x, int y, int id)
{
    for (int i = 0; i < m_levelData.getMovingPlatform().size(); ++i) {
        auto platformX = m_levelData.getMovingPlatform()[i].coords.x;
        auto platformY = m_levelData.getMovingPlatform()[i].coords.y;
        if (x == platformX && y == platformY) {
            createPlatformEntity(x, y, id, 2, m_levelData.getMovingPlatform()[i]);
        }
    }

    if (!m_levelData.getMovingPlatform().size()) {
        auto platform = createBlockEntity(x, y, id);
        platform->getComponent<SpritesheetComponent>()->setEntityWidth(
            2 * GLOBALS::ORIGINAL_CUBE_SIZE);
        platform->getComponent<TransformComponent>()->getHitbox().size.x
            = 2 * GLOBALS::SCALED_CUBE_SIZE;
    }
}

void MapSystem::create3WideMovingPlatform(int x, int y, int id)
{
    auto wide = 3;
    for (int i = 0; i < m_levelData.getMovingPlatform().size(); ++i) {
        auto platformX = m_levelData.getMovingPlatform()[i].coords.x;
        auto platformY = m_levelData.getMovingPlatform()[i].coords.y;
        if (x == platformX && y == platformY) {
            if (m_levelData.getMovingPlatform()[i].direction != Direction::None) {
                createPlatformEntity(x, y, id, wide, m_levelData.getMovingPlatform()[i]);
            }
        }
    }
}

void MapSystem::createPlatformLevel(LevelData::PlatformLevel platformLevel)
{
    auto leftCoordinate = platformLevel.leftCoords;
    auto rightCoordinare = platformLevel.rightCoords;
    auto pulleyHeight = platformLevel.yLevel + 1;

    auto leftLineX = leftCoordinate.x + 1;
    auto rightLineX = rightCoordinare.x + 1;

    auto pulleyID
        = getReferenceBlockIDAsEntity(m_backgroundMap.getLevelData()[pulleyHeight - 1][leftLineX],
                                      391);
    if (pulleyID == -1) {
        pulleyID = 391;
    }

    auto leftPulleyLine = createBackgroundEntity(leftLineX, pulleyHeight, pulleyID);
    auto rightPulleyLine = createBackgroundEntity(rightLineX, pulleyHeight, pulleyID);

    auto leftPlatform = m_world->create();
    auto rightPlatform = m_world->create();

    leftPlatform->addComponent<TransformComponent>(sf::Vector2f(leftCoordinate.x
                                                                    * GLOBALS::SCALED_CUBE_SIZE,
                                                                leftCoordinate.y
                                                                    * GLOBALS::SCALED_CUBE_SIZE),
                                                   sf::Vector2i(3, 1),
                                                   sf::Rect<int>({0, 0},
                                                                 {3 * GLOBALS::SCALED_CUBE_SIZE,
                                                                  GLOBALS::SCALED_CUBE_SIZE / 2}));
    leftPlatform->addComponent<TextureComponent>(
        Game::s_textureHolder->get(TextureID::BlockTileSheet));
    leftPlatform->addComponent<SpritesheetComponent>(3 * GLOBALS::ORIGINAL_CUBE_SIZE,
                                                     GLOBALS::ORIGINAL_CUBE_SIZE,
                                                     1,
                                                     1,
                                                     1,
                                                     GLOBALS::ORIGINAL_CUBE_SIZE,
                                                     GLOBALS::ORIGINAL_CUBE_SIZE,
                                                     Map::BlockIDCoordinates.at(809));
    leftPlatform->addComponent<MovingComponent>(sf::Vector2f(0, 0), sf::Vector2f(0, 0));
    leftPlatform->addComponent<PlatformLevelComponent>(rightPlatform,
                                                       leftPulleyLine,
                                                       pulleyHeight * GLOBALS::SCALED_CUBE_SIZE);
    leftPlatform->addComponent<ForegroundComponent>();
    leftPlatform->addComponent<TileComponent>();

    rightPlatform->addComponent<TransformComponent>(sf::Vector2f(rightCoordinare.x
                                                                     * GLOBALS::SCALED_CUBE_SIZE,
                                                                 rightCoordinare.y
                                                                     * GLOBALS::SCALED_CUBE_SIZE),
                                                    sf::Vector2i(3, 1),
                                                    sf::Rect<int>({0, 0},
                                                                  {3 * GLOBALS::SCALED_CUBE_SIZE,
                                                                   GLOBALS::SCALED_CUBE_SIZE / 2}));
    rightPlatform->addComponent<TextureComponent>(
        Game::s_textureHolder->get(TextureID::BlockTileSheet));
    rightPlatform->addComponent<SpritesheetComponent>(3 * GLOBALS::ORIGINAL_CUBE_SIZE,
                                                      GLOBALS::ORIGINAL_CUBE_SIZE,
                                                      1,
                                                      1,
                                                      1,
                                                      GLOBALS::ORIGINAL_CUBE_SIZE,
                                                      GLOBALS::ORIGINAL_CUBE_SIZE,
                                                      Map::BlockIDCoordinates.at(809));
    rightPlatform->addComponent<MovingComponent>(sf::Vector2f(0, 0), sf::Vector2f(0, 0));
    rightPlatform->addComponent<PlatformLevelComponent>(leftPlatform,
                                                        rightPulleyLine,
                                                        pulleyHeight * GLOBALS::SCALED_CUBE_SIZE);
    rightPlatform->addComponent<ForegroundComponent>();
    rightPlatform->addComponent<TileComponent>();
}

void MapSystem::createEnemyEntity(int x, int y, int entityID, int referenceID)
{
    const int KOOPA_CODE = 38;
    const int SHIFTED_KOOPA_CODE = 39;
    const int KOOPA_PARATROOPA_CODE = 40;
    const int PIRANHA_PLANT_CODE = 44;
    const int BLOOPER_CODE = 48;
    const int LAKITU_CODE = 50;
    const int HAMMER_BRO_CODE = 56;
    const int BOWSER_CODE = 61;

    const int GOOMBA_CODE = 70;
    const int SHIFTED_GOOMBA_CODE = 71;
    const int CHEEP_CHEEP_CODE = 81;
    const int BUZZY_BEETLE_CODE = 87;
    const int RED_KOOPA_CODE = 455;
    const int RED_CHEEP_CHEEP_CODE = 498;
    const int LAVA_BUBBLE_CODE = 504;

    if (referenceID == KOOPA_CODE || referenceID == RED_KOOPA_CODE) {
        createKoopa(x, y, entityID);
    } else if (referenceID == SHIFTED_KOOPA_CODE) {
        createShiftedKoopa(x, y, entityID);
    } else if (referenceID == KOOPA_PARATROOPA_CODE) {
        createKoopaParatroopa(x, y, entityID);
    } else if (referenceID == PIRANHA_PLANT_CODE) {
        createPirhannaPlant(x, y, entityID);
    } else if (referenceID == BLOOPER_CODE) {
        createBlooper(x, y, entityID);
    } else if (referenceID == LAKITU_CODE) {
        createLakitu(x, y, entityID);
    } else if (referenceID == HAMMER_BRO_CODE) {
        createHammerBro(x, y, entityID);
    } else if (referenceID == BOWSER_CODE) {
        createBowser(x, y, entityID);
    } else if (referenceID == GOOMBA_CODE) {
        createGoomba(x, y, entityID);
    } else if (referenceID == SHIFTED_GOOMBA_CODE) {
        createShiftedGoomba(x, y, entityID);
    } else if (referenceID == CHEEP_CHEEP_CODE) {
        createCheepCheep(x, y, entityID);
    } else if (referenceID == BUZZY_BEETLE_CODE) {
        createBuzzyBeetle(x, y, entityID);
    } else if (referenceID == RED_CHEEP_CHEEP_CODE) {
        createRedCheepCheep(x, y, entityID);
    } else if (referenceID == LAVA_BUBBLE_CODE) {
        createLavaBubble(x, y, entityID);
    }
}

void MapSystem::createKoopa(int x, int y, int id)
{
    auto entity = m_world->create();
    entity->addComponent<TransformComponent>(sf::Vector2f(x * GLOBALS::SCALED_CUBE_SIZE,
                                                          y * GLOBALS::SCALED_CUBE_SIZE),
                                             sf::Vector2(1, 2),
                                             sf::Rect<int>({0, 0},
                                                           {GLOBALS::SCALED_CUBE_SIZE,
                                                            2 * GLOBALS::SCALED_CUBE_SIZE}));

    entity->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::EnemySpriteSheet));
    entity->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                               2 * GLOBALS::ORIGINAL_CUBE_SIZE,
                                               1,
                                               1,
                                               0,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               Map::EnemyIDCoordinates.at(id));
    const std::vector<int> frameIDs{id, id + 1};
    const int framesPerSecond = 6;
    entity->addComponent<AnimationComponent>(frameIDs, framesPerSecond, Map::EnemyIDCoordinates);
    entity->addComponent<MovingComponent>(sf::Vector2f(-GLOBALS::ENEMY_SPEED, 0.0f),
                                          sf::Vector2f(0.0f, 0.0f));

    entity->addComponent<CrushableComponent>([=](Entity *entity) {
        entity->remove<AnimationComponent>();
        entity->getComponent<EnemyComponent>()->setType(EnemyType::KoopaShell);
        entity->getComponent<MovingComponent>()->getVelocity().x = 0.0f;
        entity->getComponent<SpritesheetComponent>()->setEntityHeight(GLOBALS::ORIGINAL_CUBE_SIZE);
        entity->addComponent<DestroyOutsideCameraComponent>();
        auto transform = entity->getComponent<TransformComponent>();
        transform->setHitbox(0, 0, GLOBALS::SCALED_CUBE_SIZE, GLOBALS::SCALED_CUBE_SIZE);
        transform->move(sf::Vector2f(0.0f, GLOBALS::SCALED_CUBE_SIZE));
        transform->getSize().y = 1;
        entity->getComponent<SpritesheetComponent>()->setSpritesheetCoordinates(
            Map::EnemyIDCoordinates.at(id + 39));
    });

    entity->addComponent<GravityComponent>();
    entity->addComponent<EnemyComponent>(EnemyType::Koopa);
}

void MapSystem::createKoopaParatroopa(int x, int y, int id)
{
    auto koopa = m_world->create();
    koopa->addComponent<TransformComponent>(sf::Vector2f(x * GLOBALS::SCALED_CUBE_SIZE,
                                                         y * GLOBALS::SCALED_CUBE_SIZE),
                                            sf::Vector2i(1, 2),
                                            sf::Rect<int>({0, GLOBALS::SCALED_CUBE_SIZE},
                                                          {GLOBALS::SCALED_CUBE_SIZE,
                                                           GLOBALS::SCALED_CUBE_SIZE}));
    koopa->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::EnemySpriteSheet));
    koopa->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                              2 * GLOBALS::ORIGINAL_CUBE_SIZE,
                                              1,
                                              1,
                                              0,
                                              GLOBALS::ORIGINAL_CUBE_SIZE,
                                              GLOBALS::ORIGINAL_CUBE_SIZE,
                                              Map::EnemyIDCoordinates.at(id));
    std::vector<int> frameIDs{id, id + 1};
    int framesPerSecond = 4;
    koopa->addComponent<AnimationComponent>(frameIDs, framesPerSecond, Map::EnemyIDCoordinates);
    koopa->addComponent<MovingComponent>(sf::Vector2f(-GLOBALS::ENEMY_SPEED, 0.0f),
                                         sf::Vector2f(0.0f, 0.0f));
    koopa->addComponent<GravityComponent>();
    koopa->addComponent<FrictionExemptComponent>();
    koopa->addComponent<WaitUntilComponent>(
        [=](Entity *entity) { return entity->hasComponent<BottomCollisionComponent>(); },
        [=](Entity *entity) {
            entity->remove<BottomCollisionComponent>();
            entity->getComponent<MovingComponent>()->getVelocity().y = -8.f;
            entity->getComponent<MovingComponent>()->getAcceleration().y = -0.22f;
        });

    koopa->addComponent<CrushableComponent>([=](Entity *entity) {
        if (entity->getComponent<EnemyComponent>()->type() == EnemyType::KoopaParatroopa) {
            entity->remove<WaitUntilComponent>();
            entity->getComponent<EnemyComponent>()->setType(EnemyType::Koopa);
            std::vector<int> newFrameIDs{id - 2, id - 1};
            entity->getComponent<AnimationComponent>()->setFrameIDs(newFrameIDs);
            entity->getComponent<MovingComponent>()->getVelocity().y = 0.0f;
            //entity->getComponent<SpritesheetComponent>()->setEntityHeight(GLOBALS::ORIGINAL_CUBE_SIZE);
            entity->addComponent<DestroyOutsideCameraComponent>();
        } else if (entity->getComponent<EnemyComponent>()->type() == EnemyType::Koopa) {
            entity->remove<AnimationComponent>();
            entity->getComponent<EnemyComponent>()->setType(EnemyType::KoopaShell);
            entity->getComponent<MovingComponent>()->getVelocity().x = 0.0f;
            entity->getComponent<SpritesheetComponent>()->setEntityHeight(
                GLOBALS::ORIGINAL_CUBE_SIZE);
            entity->addComponent<DestroyOutsideCameraComponent>();
            auto transform = entity->getComponent<TransformComponent>();
            //position.scale.y = SCALED_CUBE_SIZE
            transform->setHitbox(0, 0, GLOBALS::SCALED_CUBE_SIZE, GLOBALS::SCALED_CUBE_SIZE);
            transform->move(sf::Vector2f(0.0f, GLOBALS::SCALED_CUBE_SIZE));
            transform->getSize().y = 1;
            entity->getComponent<SpritesheetComponent>()->setSpritesheetCoordinates(
                Map::EnemyIDCoordinates.at(id + 37));
        }
    });

    koopa->addComponent<EnemyComponent>(EnemyType::KoopaParatroopa);
}

void MapSystem::createShiftedKoopa(int x, int y, int id)
{
    auto entity = m_world->create();
    entity->addComponent<TransformComponent>(sf::Vector2f(x * GLOBALS::SCALED_CUBE_SIZE,
                                                          y * GLOBALS::SCALED_CUBE_SIZE),
                                             sf::Vector2(1, 2),
                                             sf::Rect<int>({0, 0},
                                                           {GLOBALS::SCALED_CUBE_SIZE,
                                                            2 * GLOBALS::SCALED_CUBE_SIZE}));

    entity->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::EnemySpriteSheet));
    entity->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                               2 * GLOBALS::ORIGINAL_CUBE_SIZE,
                                               1,
                                               1,
                                               0,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               Map::EnemyIDCoordinates.at(id));
    auto firstAnimationID = id - 1;
    const std::vector<int> frameIDs{firstAnimationID + 1, firstAnimationID};
    const int framesPerSecond = 6;
    entity->addComponent<AnimationComponent>(frameIDs, framesPerSecond, Map::EnemyIDCoordinates);
    entity->addComponent<MovingComponent>(sf::Vector2f(-GLOBALS::ENEMY_SPEED, 0.0f),
                                          sf::Vector2f(0.0f, 0.0f));

    entity->addComponent<CrushableComponent>([=](Entity *entity) {
        entity->remove<AnimationComponent>();
        entity->getComponent<EnemyComponent>()->setType(EnemyType::KoopaShell);
        entity->getComponent<MovingComponent>()->getVelocity().x = 0.0f;
        entity->getComponent<SpritesheetComponent>()->setEntityHeight(GLOBALS::ORIGINAL_CUBE_SIZE);
        entity->addComponent<DestroyOutsideCameraComponent>();
        auto transform = entity->getComponent<TransformComponent>();
        //position.scale.y = SCALED_CUBE_SIZE
        transform->setHitbox(0, 0, GLOBALS::SCALED_CUBE_SIZE, GLOBALS::SCALED_CUBE_SIZE);
        transform->move(sf::Vector2f(0.0f, GLOBALS::SCALED_CUBE_SIZE));
        transform->getSize().y = 1;
        entity->getComponent<SpritesheetComponent>()->setSpritesheetCoordinates(
            Map::EnemyIDCoordinates.at(id + 38));
    });

    entity->addComponent<GravityComponent>();
    entity->addComponent<EnemyComponent>(EnemyType::Koopa);
}

void MapSystem::createGoomba(int x, int y, int id)
{
    auto entity = m_world->create();
    entity->addComponent<TransformComponent>(
        sf::Vector2f(x * GLOBALS::SCALED_CUBE_SIZE, y * GLOBALS::SCALED_CUBE_SIZE));
    entity->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::EnemySpriteSheet));
    entity->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               1,
                                               1,
                                               0,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               Map::EnemyIDCoordinates.at(id));

    const std::vector<int> frameIDs{id, id + 1};
    const int framesPerSecond = 8;
    entity->addComponent<AnimationComponent>(frameIDs, framesPerSecond, Map::EnemyIDCoordinates);
    entity->addComponent<MovingComponent>(sf::Vector2f(-GLOBALS::ENEMY_SPEED, 0.0f),
                                          sf::Vector2f(0.0f, 0.0f));
    entity->addComponent<CrushableComponent>([=](Entity *entity) {
        entity->remove<AnimationComponent>();
        entity->getComponent<SpritesheetComponent>()->setSpritesheetCoordinates(
            Map::EnemyIDCoordinates.at(id + 2));
        entity->addComponent<DeadComponent>();
        entity->addComponent<FrozenComponent>();
        entity->addComponent<DestroyDelayedComponent>(20);
    });

    entity->addComponent<GravityComponent>();
    entity->addComponent<EnemyComponent>(EnemyType::Goomba);
}

void MapSystem::createShiftedGoomba(int x, int y, int id)
{
    auto entity = m_world->create();
    entity->addComponent<TransformComponent>(
        sf::Vector2f(x * GLOBALS::SCALED_CUBE_SIZE, y * GLOBALS::SCALED_CUBE_SIZE));
    entity->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::EnemySpriteSheet));
    entity->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               1,
                                               1,
                                               0,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               Map::EnemyIDCoordinates.at(id));

    auto firstAnimationID = id - 1;
    const std::vector<int> frameIDs{firstAnimationID + 1, firstAnimationID};
    const int framesPerSecond = 8;
    entity->addComponent<AnimationComponent>(frameIDs, framesPerSecond, Map::EnemyIDCoordinates);
    entity->addComponent<MovingComponent>(sf::Vector2f(-GLOBALS::ENEMY_SPEED, 0.0f),
                                          sf::Vector2f(0.0f, 0.0f));
    entity->addComponent<CrushableComponent>([=](Entity *entity) {
        entity->remove<AnimationComponent>();
        entity->getComponent<SpritesheetComponent>()->setSpritesheetCoordinates(
            Map::EnemyIDCoordinates.at(id + 1));
        entity->addComponent<DeadComponent>();
        entity->addComponent<FrozenComponent>();
        entity->addComponent<DestroyDelayedComponent>(20);
    });

    entity->addComponent<GravityComponent>();
    entity->addComponent<EnemyComponent>(EnemyType::Goomba);
}

void MapSystem::createBowser(int x, int y, int id)
{
    auto bowser = m_world->create();
    auto transform = bowser->addComponent<TransformComponent>(
        sf::Vector2f(x * GLOBALS::SCALED_CUBE_SIZE, y * GLOBALS::SCALED_CUBE_SIZE),
        sf::Vector2i(2, 2),
        sf::Rect<int>{{0, 0}, {2 * GLOBALS::SCALED_CUBE_SIZE, 2 * GLOBALS::SCALED_CUBE_SIZE}});
    auto texture = bowser->addComponent<TextureComponent>(
        Game::s_textureHolder->get(TextureID::EnemySpriteSheet));
    bowser->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE * 2,
                                               GLOBALS::ORIGINAL_CUBE_SIZE * 2,
                                               1,
                                               1,
                                               0,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               Map::EnemyIDCoordinates.at(id));
    auto move = bowser->addComponent<MovingComponent>(sf::Vector2f(0.0f, 0.0f),
                                                      sf::Vector2f(0.0f, -0.3f));
    bowser->addComponent<GravityComponent>();
    bowser->addComponent<FrictionExemptComponent>();

    auto mouthOpenID = getReferenceEnemyIDAsEntity(id, 61);
    auto mouthCloseID = getReferenceEnemyIDAsEntity(id, 65);
    auto hammerID = getReferenceEnemyIDAsEntity(id, 60);

    std::vector<int> mouthOpenAnim{mouthOpenID, mouthOpenID + 2};
    std::vector<int> mouthClosedAnim{mouthCloseID, mouthCloseID + 2};

    auto animation = bowser->addComponent<AnimationComponent>(mouthOpenAnim,
                                                              2,
                                                              Map::EnemyIDCoordinates);

    auto bowserMovement = [=](Entity *entity) {
        auto bowserComponent = entity->getComponent<BowserComponent>();
        if (bowserComponent->getLastMoveDirection() == Direction::Left) {
            move->getVelocity().x = 1.0f;
            bowserComponent->getLastMoveDirection() = Direction::Right;
        } else {
            move->getVelocity().x = 1.0f;
            bowserComponent->getLastMoveDirection() = Direction::Left;
        }
        bowserComponent->getLastMoveTime() = 0;
    };

    auto bowserStop = [=](Entity *entity) {
        move->getVelocity().x = 0;
        entity->getComponent<BowserComponent>()->getlastStopTime() = 0;
    };

    auto bowserJump = [=](Entity *entity) {
        auto bowserComponent = entity->getComponent<BowserComponent>();
        move->getVelocity().y = -5.0f;
        move->getAcceleration().y = -0.35f;
        bowserComponent->getLastJumpTime() = 0;
    };

    std::vector<std::function<void(Entity *)>> bowserMovements{bowserMovement,
                                                               bowserStop,
                                                               bowserJump};

    auto bowserFire = [=](Entity *entity, int number = 0) {
        animation->setFrameIDs(mouthClosedAnim);
        entity->addComponent<CallbackComponent>(
            [=](Entity *entity) {
                animation->setFrameIDs(mouthOpenAnim);
                auto blastSound = m_world->create();
                blastSound->addComponent<SoundComponent>(SoundID::BowserFire);

                //local blastSound = Concord.entity(self.world)
                // blastSound:give('sound_component', SOUND_ID.BOWSER_FIRE)
                auto fireBlast = m_world->create();
                float fireBastYPoses[2]{transform->getTop() + 4,
                                        transform->getTop() + GLOBALS::SCALED_CUBE_SIZE + 4};
                auto blastPosition = fireBlast->addComponent<TransformComponent>(
                    sf::Vector2f(0.0f, fireBastYPoses[rand() % 2]));
                auto blastTexture = fireBlast->addComponent<TextureComponent>(
                    Game::s_textureHolder->get(TextureID::EnemySpriteSheet));
                fireBlast->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE
                                                                  + GLOBALS::ORIGINAL_CUBE_SIZE / 2,
                                                              GLOBALS::ORIGINAL_CUBE_SIZE / 2,
                                                              1,
                                                              1,
                                                              0,
                                                              GLOBALS::ORIGINAL_CUBE_SIZE,
                                                              GLOBALS::ORIGINAL_CUBE_SIZE,
                                                              Map::EnemyIDCoordinates.at(470));
                std::vector<int> frameIDs{470, 505};
                fireBlast->addComponent<AnimationComponent>(frameIDs, 16, Map::EnemyIDCoordinates);
                auto blastMove = fireBlast->addComponent<MovingComponent>(sf::Vector2f(0.0f, 0.0f),
                                                                          sf::Vector2f(0.0f, 0.0f));
                fireBlast->addComponent<FrictionExemptComponent>();
                fireBlast->addComponent<DestroyOutsideCameraComponent>();
                if (texture->isHorizontalFlipped()) {
                    blastMove->getVelocity().x = 3;
                    blastPosition->setLeft(transform->getRight());
                    blastTexture->setHorizontalFlipped(true);
                } else {
                    blastMove->getVelocity().x = -3;
                    blastPosition->setRight(transform->getLeft());
                    blastTexture->setHorizontalFlipped(false);
                }
                fireBlast->addComponent<ProjectileComponent>(ProjectileType::Other);
            },
            2 * GLOBALS::FPS);
        entity->getComponent<BowserComponent>()->getLastAttackTime() = 0;
    };

    auto bowserHammer = [=](Entity *entity, int number = 0) {
        for (int i = 0; i < number; ++i) {
            auto hammer = m_world->create();
            hammer->addComponent<CallbackComponent>(
                [=](Entity *entity) {
                    auto hammerTransform = entity->addComponent<TransformComponent>(
                        sf::Vector2f(transform->getLeft(), transform->getTop()));
                    if (texture->isHorizontalFlipped()) {
                        hammerTransform->setLeft(transform->getRight());
                    } else {
                        hammerTransform->setRight(transform->getLeft());
                    }

                    entity->addComponent<TextureComponent>(
                        Game::s_textureHolder->get(TextureID::EnemySpriteSheet));

                    entity->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                                               1,
                                                               1,
                                                               0,
                                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                                               Map::EnemyIDCoordinates.at(hammerID));
                    float randomXVelocity = -((float) ((float) rand() / (float) RAND_MAX) + 2.25);
                    float randomYVelocity = -((float) ((float) rand() / (float) RAND_MAX) * 0.5 + 6);

                    if (texture->isHorizontalFlipped()) {
                        randomXVelocity *= -1;
                    }

                    entity->addComponent<MovingComponent>(sf::Vector2f(randomXVelocity,
                                                                       randomYVelocity),
                                                          sf::Vector2f(0.0f, -0.35f));
                    entity->addComponent<FrictionExemptComponent>();
                    entity->addComponent<GravityComponent>();
                    entity->addComponent<DestroyOutsideCameraComponent>();
                    entity->addComponent<ProjectileComponent>(ProjectileType::Other);
                    entity->addComponent<ParticleComponent>();
                },
                i * 4);
        }
        entity->getComponent<BowserComponent>()->getLastAttackTime() = 0;
    };

    std::vector<std::function<void(Entity *, int number)>> attacks{bowserFire, bowserHammer};

    bowser->addComponent<BowserComponent>(attacks, bowserMovements);
    bowser->addComponent<EnemyComponent>(EnemyType::Bowser);
}

void MapSystem::createPirhannaPlant(int x, int y, int id)
{
    auto pirhanna = m_world->create();
    pirhanna->addComponent<TransformComponent>(sf::Vector2f(x * GLOBALS::SCALED_CUBE_SIZE
                                                                + 0.5 * GLOBALS::SCALED_CUBE_SIZE,
                                                            y * GLOBALS::SCALED_CUBE_SIZE),
                                               sf::Vector2i{1, 2},
                                               sf::Rect<int>{{24, 48}, {16, 16}});
    pirhanna->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                                 2 * GLOBALS::ORIGINAL_CUBE_SIZE,
                                                 1,
                                                 1,
                                                 0,
                                                 GLOBALS::ORIGINAL_CUBE_SIZE,
                                                 GLOBALS::ORIGINAL_CUBE_SIZE,
                                                 Map::EnemyIDCoordinates.at(id));
    std::vector frameID{id, id + 1};
    pirhanna->addComponent<AnimationComponent>(frameID, 4, Map::EnemyIDCoordinates);
    pirhanna->addComponent<MovingComponent>(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(0.0f, 0.0f));
    pirhanna->addComponent<MoveOutsideCameraComponent>();
    pirhanna->addComponent<TextureComponent>(
        Game::s_textureHolder->get(TextureID::EnemySpriteSheet));
    pirhanna->addComponent<CollisionExemptComponent>();
    pirhanna->addComponent<FrictionExemptComponent>();
    pirhanna->addComponent<EnemyComponent>(EnemyType::PiranhaPlant);
    pirhanna->addComponent<PiranhaPlantComponent>();

    auto pirhannaComponent = pirhanna->getComponent<PiranhaPlantComponent>();
    pirhannaComponent->getPipeCoordinates().x
        = pirhanna->getComponent<TransformComponent>()->getPosition().x;
    pirhannaComponent->getPipeCoordinates().y
        = pirhanna->getComponent<TransformComponent>()->getPosition().y
          + 2 * GLOBALS::SCALED_CUBE_SIZE;

    pirhanna->addComponent<TimerComponent>(
        [](Entity *entity) {
            auto pirhanna = entity->getComponent<PiranhaPlantComponent>();
            if (pirhanna->getInPipe()) {
                entity->getComponent<MovingComponent>()->getVelocity().y = -1.0f;
                entity->addComponent<WaitUntilComponent>(
                    [=](Entity *entity) {
                        return entity->getComponent<TransformComponent>()->getPosition().y
                               <= pirhanna->getPipeCoordinates().y - 2 * GLOBALS::SCALED_CUBE_SIZE;
                    },
                    [=](Entity *entity) {
                        entity->getComponent<TransformComponent>()->setBottom(
                            pirhanna->getPipeCoordinates().y);
                        entity->getComponent<MovingComponent>()->getVelocity().y = 0.0f;
                        entity->remove<WaitUntilComponent>();
                    });

                pirhanna->getInPipe() = false;
            } else {
                entity->getComponent<MovingComponent>()->getVelocity().y = 1.0f;
                entity->addComponent<WaitUntilComponent>(
                    [=](Entity *entity) {
                        return entity->getComponent<TransformComponent>()->getPosition().y
                               >= pirhanna->getPipeCoordinates().y;
                    },
                    [=](Entity *entity) {
                        entity->getComponent<TransformComponent>()->setTop(
                            pirhanna->getPipeCoordinates().y);
                        entity->getComponent<MovingComponent>()->getVelocity().y = 0.0f;
                        entity->remove<WaitUntilComponent>();
                    });
                pirhanna->getInPipe() = true;
            }
        },
        3 * GLOBALS::FPS);
}

void MapSystem::createBlooper(int x, int y, int id)
{
    auto blooper = m_world->create();
    auto transform = blooper->addComponent<TransformComponent>(
        sf::Vector2f(x * GLOBALS::SCALED_CUBE_SIZE, y * GLOBALS::SCALED_CUBE_SIZE),
        sf::Vector2i(1, 2),
        sf::Rect<int>({0, GLOBALS::SCALED_CUBE_SIZE / 2},
                      {GLOBALS::SCALED_CUBE_SIZE, GLOBALS::SCALED_CUBE_SIZE}));
    blooper->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::EnemySpriteSheet));
    blooper->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                                2 * GLOBALS::ORIGINAL_CUBE_SIZE,
                                                1,
                                                1,
                                                0,
                                                GLOBALS::ORIGINAL_CUBE_SIZE,
                                                GLOBALS::ORIGINAL_CUBE_SIZE,
                                                Map::EnemyIDCoordinates.at(id));

    std::vector<int> frameIDs{id, id + 1};
    int framesPerSecond = 2;
    blooper->addComponent<AnimationComponent>(frameIDs, framesPerSecond, Map::EnemyIDCoordinates);
    auto move = blooper->addComponent<MovingComponent>(sf::Vector2f(0.0f, 0.0f),
                                                       sf::Vector2f(0.0f, 0.0f));
    blooper->addComponent<GravityComponent>();
    blooper->addComponent<FrictionExemptComponent>();
    blooper->addComponent<CollisionExemptComponent>();
    blooper->addComponent<TimerComponent>(
        [=](Entity *entity) {
            if (!Camera::GetInstance().inCameraRange(transform)) {
                return;
            }
            if (entity->hasComponent<DeadComponent>()) {
                entity->remove<CallbackComponent>();
                entity->remove<TimerComponent>();
                move->getVelocity().y = 0.0f;
                move->getAcceleration().y = 0.0f;
                return;
            }

            entity->remove<GravityComponent>();
            move->getAcceleration().y = 0.0f;

            const float speed = 2.2f;
            auto player = m_world->findFirst<PlayerComponent>();
            auto playerTransform = player->getComponent<TransformComponent>();
            if (playerTransform->getPosition().x > transform->getPosition().x) {
                move->getVelocity().x = speed;
            } else {
                move->getVelocity().x = -speed;
            }

            if (transform->getPosition().y < Camera::GetInstance().getCameraCenterY()) {
                move->getVelocity().y = speed;
            } else {
                move->getVelocity().y = -speed;
            }

            entity->addComponent<CallbackComponent>(
                [=](Entity *entity) {
                    entity->addComponent<GravityComponent>();
                    move->getVelocity().x = 0.0f;
                    move->getVelocity().y = 0.0f;
                },
                GLOBALS::FPS / 2.0f);
        },
        GLOBALS::SCALED_CUBE_SIZE);

    blooper->addComponent<EnemyComponent>(EnemyType::Blooper);
}

void MapSystem::createCheepCheep(int x, int y, int id)
{
    auto entity = m_world->create();
    entity->addComponent<TransformComponent>(
        sf::Vector2f(x * GLOBALS::SCALED_CUBE_SIZE, y * GLOBALS::SCALED_CUBE_SIZE));
    entity->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::EnemySpriteSheet));
    entity->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               1,
                                               1,
                                               0,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               Map::EnemyIDCoordinates.at(id));
    std::vector<int> frameIDs{id, id + 1};
    int framesPerSecond = 6;
    entity->addComponent<AnimationComponent>(frameIDs, framesPerSecond, Map::EnemyIDCoordinates);
    entity->addComponent<MovingComponent>(sf::Vector2f(-GLOBALS::ENEMY_SPEED, 0.0f),
                                          sf::Vector2f(0.0f, 0.0f));
    entity->addComponent<CollisionExemptComponent>();
    entity->addComponent<FrictionExemptComponent>();
    entity->addComponent<EnemyComponent>(EnemyType::CheepCheep);
}

void MapSystem::createRedCheepCheep(int x, int y, int id)
{
    if (getLevelData().getLevelType() == LevelType::Underwater
        || getLevelData().getLevelType() == LevelType::StartUnderground) {
        createCheepCheep(x, y, id);
    } else { //If in those flying patches of them idk what they're called
        auto entity = m_world->create();
        auto transform = entity->addComponent<TransformComponent>(
            sf::Vector2f(x * GLOBALS::SCALED_CUBE_SIZE, (y + 1) * GLOBALS::SCALED_CUBE_SIZE));
        auto texture = entity->addComponent<TextureComponent>(
            Game::s_textureHolder->get(TextureID::EnemySpriteSheet));
        texture->setHorizontalFlipped(true);
        entity->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                                   GLOBALS::ORIGINAL_CUBE_SIZE,
                                                   1,
                                                   1,
                                                   0,
                                                   GLOBALS::ORIGINAL_CUBE_SIZE,
                                                   GLOBALS::ORIGINAL_CUBE_SIZE,
                                                   Map::EnemyIDCoordinates.at(id));
        std::vector<int> frameIDs{id, id + 1};
        int framesPerSecond = 6;
        entity->addComponent<AnimationComponent>(frameIDs, framesPerSecond, Map::EnemyIDCoordinates);
        auto move = entity->addComponent<MovingComponent>(sf::Vector2f(0.0f, 0.0f),
                                                          sf::Vector2f(0.0f, 0.0f));
        entity->addComponent<MoveOutsideCameraComponent>();
        entity->addComponent<GravityComponent>();
        entity->addComponent<CollisionExemptComponent>();
        entity->addComponent<FrictionExemptComponent>();

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dist(0.5, 2.0);

        float randomValue = dist(gen);
        entity->addComponent<TimerComponent>(
            [=](Entity *entity) {
                entity->addComponent<CallbackComponent>(
                    [=](Entity *entity) {
                        if (Camera::GetInstance().inCameraYRange(transform)) {
                            return;
                        }

                        auto newTransform = entity->addComponent<TransformComponent>(
                            sf::Vector2f(x * GLOBALS::SCALED_CUBE_SIZE,
                                         (y + 1) * GLOBALS::SCALED_CUBE_SIZE));

                        if (!Camera::GetInstance().inCameraXRange(newTransform)) {
                            return;
                        }
                        move->getVelocity().x = randomValue * 2.0f;
                        move->getVelocity().y = -10.0f;
                        move->getAcceleration().y = -0.4542f;
                    },
                    GLOBALS::FPS * randomValue);
            },
            2.5f * GLOBALS::FPS);

        entity->addComponent<CrushableComponent>([=](Entity *entity) {
            entity->getComponent<TextureComponent>()->setVerticalFlipped(true);
            entity->getComponent<MovingComponent>()->getAcceleration().y = 0.0f;
            entity->addComponent<DeadComponent>();
            entity->addComponent<ParticleComponent>();
            entity->remove<TimerComponent>();
        });

        entity->addComponent<EnemyComponent>(EnemyType::CheepCheep);
    }
}

void MapSystem::createLavaBubble(int x, int y, int id)
{
    auto entity = m_world->create();
    entity->addComponent<TransformComponent>(
        sf::Vector2f(x * GLOBALS::SCALED_CUBE_SIZE, (y + 1) * GLOBALS::SCALED_CUBE_SIZE));
    auto resetYLevel = (y + 1) * GLOBALS::SCALED_CUBE_SIZE;
    entity->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::EnemySpriteSheet));
    entity->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               1,
                                               1,
                                               0,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               Map::EnemyIDCoordinates.at(id));
    entity->addComponent<MovingComponent>(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(0.0f, 0.0f));
    entity->addComponent<ParticleComponent>();
    entity->addComponent<GravityComponent>();
    entity->addComponent<TimerComponent>(
        [=](Entity *entity) {
            entity->getComponent<TransformComponent>()->setTop(resetYLevel);
            entity->getComponent<MovingComponent>()->getVelocity().y = -10.0f;
            entity->getComponent<MovingComponent>()->getAcceleration().y = -0.4f;
        },
        GLOBALS::FPS * 4);

    entity->addComponent<EnemyComponent>(EnemyType::LavaBubble);
}

void MapSystem::createHammerBro(int x, int y, int id)
{
    auto entity = m_world->create();
    auto transform = entity->addComponent<TransformComponent>(
        sf::Vector2f(x * GLOBALS::SCALED_CUBE_SIZE, y * GLOBALS::SCALED_CUBE_SIZE),
        sf::Vector2i(1, 2),
        sf::Rect<int>({0, 0}, {GLOBALS::SCALED_CUBE_SIZE, 2 * GLOBALS::SCALED_CUBE_SIZE}));
    auto texture = entity->addComponent<TextureComponent>(
        Game::s_textureHolder->get(TextureID::EnemySpriteSheet));
    entity->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                               2 * GLOBALS::ORIGINAL_CUBE_SIZE,
                                               1,
                                               1,
                                               0,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               Map::EnemyIDCoordinates.at(id));
    entity->addComponent<MovingComponent>(sf::Vector2f(2.0f, 0.0f), sf::Vector2f(0.0f, 0.0f));
    entity->addComponent<GravityComponent>();
    entity->addComponent<FrictionExemptComponent>();
    auto armsBackID = getReferenceEnemyIDAsEntity(id, 58);
    auto hammerID = getReferenceEnemyIDAsEntity(id, 60);

    std::vector<int> armsDownAnimation{id, id + 1};
    std::vector<int> armsBackAnimation{armsBackID, armsBackID + 1};
    auto animation = entity->addComponent<AnimationComponent>(armsBackAnimation,
                                                              4,
                                                              Map::EnemyIDCoordinates);
    auto throwHammer = [=](Entity *entity) {
        animation->setFrameIDs(armsBackAnimation);
        if (!entity->hasComponent<HammerBroComponent>()) {
            return;
        }

        Entity *hammer = m_world->create();
        auto hammerPosition = hammer->addComponent<TransformComponent>(
            sf::Vector2f(0.0f, transform->getPosition().y - 10.0f));
        hammerPosition->setCenterX(transform->getCenterX());
        auto hammerTexture = hammer->addComponent<TextureComponent>(
            Game::s_textureHolder->get(TextureID::EnemySpriteSheet));
        hammerTexture->setHorizontalFlipped(texture->isHorizontalFlipped());
        hammer->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                                   GLOBALS::ORIGINAL_CUBE_SIZE,
                                                   1,
                                                   1,
                                                   0,
                                                   GLOBALS::ORIGINAL_CUBE_SIZE,
                                                   GLOBALS::ORIGINAL_CUBE_SIZE,
                                                   Map::EnemyIDCoordinates.at(hammerID));
        hammer->addComponent<ParticleComponent>();
        hammer->addComponent<DestroyOutsideCameraComponent>();

        auto hammerBroComponent = entity->getComponent<HammerBroComponent>();
        hammerBroComponent->setHammer(hammer);
        entity->addComponent<CallbackComponent>(
            [=](Entity *entity) {
                //Fail safe in case if crushed before hammer is thrown
                if (!entity->hasComponent<HammerBroComponent>()) {
                    m_world->destroy(hammer);
                    return;
                }
                animation->setFrameIDs(armsDownAnimation);
                hammerPosition->setBottom(transform->getTop());
                float xMoveVal = 0.0f;
                if (texture->isHorizontalFlipped()) {
                    hammerPosition->setLeft(transform->getRight());
                    xMoveVal = 3.0f;
                } else {
                    hammerPosition->setRight(transform->getLeft());
                    xMoveVal = -3.0f;
                }

                hammer->addComponent<MovingComponent>(sf::Vector2f(xMoveVal, -6.0f),
                                                      sf::Vector2f(0.0f, 0.0f));
                hammer->addComponent<FrictionExemptComponent>();
                hammer->addComponent<GravityComponent>();
                hammer->addComponent<ProjectileComponent>(ProjectileType::Other);
            },
            0.25f * GLOBALS::FPS);
    };

    entity->addComponent<HammerBroComponent>(throwHammer);
    entity->addComponent<CrushableComponent>([=](Entity *entity) {
        entity->getComponent<TextureComponent>()->setVerticalFlipped(true);
        entity->addComponent<ParticleComponent>();
        entity->remove<CallbackComponent>();
        entity->remove<HammerBroComponent>();
        entity->addComponent<DeadComponent>();
    });

    entity->addComponent<EnemyComponent>(EnemyType::HammerBro);
}

void MapSystem::createLakitu(int x, int y, int id)
{
    auto lakitu = m_world->create();
    lakitu->addComponent<TransformComponent>(sf::Vector2f(x * GLOBALS::SCALED_CUBE_SIZE,
                                                          y * GLOBALS::SCALED_CUBE_SIZE),
                                             sf::Vector2i(1, 2),
                                             sf::Rect<int>({0, 0},
                                                           {GLOBALS::SCALED_CUBE_SIZE,
                                                            2 * GLOBALS::SCALED_CUBE_SIZE}));
    lakitu->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::EnemySpriteSheet));
    lakitu->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                               2 * GLOBALS::ORIGINAL_CUBE_SIZE,
                                               1,
                                               1,
                                               0,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               Map::EnemyIDCoordinates.at(id));
    lakitu->addComponent<MovingComponent>(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(0.0f, 0.0f));
    lakitu->addComponent<FrictionExemptComponent>();
    lakitu->addComponent<CrushableComponent>([=](Entity *entity) {
        entity->getComponent<TextureComponent>()->setVerticalFlipped(true);
        entity->addComponent<DeadComponent>();
        entity->addComponent<GravityComponent>();
        entity->addComponent<ParticleComponent>();
    });

    lakitu->addComponent<LakituComponent>();
    lakitu->addComponent<EnemyComponent>(EnemyType::Lakitu);
    auto inCloudID = getReferenceEnemyIDAsEntity(id, 86);
    auto createSpine = [=](Entity *entity) {
        auto transform = entity->getComponent<TransformComponent>();
        auto texture = entity->getComponent<TextureComponent>();
        auto spine = m_world->create();
        spine->addComponent<TransformComponent>(transform->getPosition());
        spine->addComponent<TextureComponent>(
            Game::s_textureHolder->get(TextureID::EnemySpriteSheet));
        spine->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                                  GLOBALS::ORIGINAL_CUBE_SIZE,
                                                  1,
                                                  1,
                                                  0,
                                                  GLOBALS::ORIGINAL_CUBE_SIZE,
                                                  GLOBALS::ORIGINAL_CUBE_SIZE,
                                                  Map::EnemyIDCoordinates.at(500));
        std::vector<int> frameIDs{500, 501};
        int framesPerSecond = 4;
        spine->addComponent<AnimationComponent>(frameIDs, framesPerSecond, Map::EnemyIDCoordinates);
        auto xMove = 0.0f;
        if (texture->isHorizontalFlipped()) {
            xMove = 2.5f;
        } else {
            xMove = -2.5f;
        }

        spine->addComponent<MovingComponent>(sf::Vector2f(xMove, 0.0f), sf::Vector2f(0.0f, 0.0f));
        spine->addComponent<GravityComponent>();
        spine->addComponent<DestroyOutsideCameraComponent>();
        spine->addComponent<EnemyComponent>(EnemyType::Spine);
        return spine;
    };

    auto throwSpine = [=](Entity *entity) {
        auto transform = entity->getComponent<TransformComponent>();
        auto spritesheet = entity->getComponent<SpritesheetComponent>();
        if (!Camera::GetInstance().inCameraRange(transform)) {
            return;
        }
        CommandScheduler::getInstance().addCommand(new SequenceCommand(
            //Set Lakitu to be in the cloud
            {new RunCommand([=, this]() {
                 if (!m_world->hasEntity(entity)) {
                     return;
                 }
                 transform->getSize().y = 1;
                 transform->move(sf::Vector2f(0.0f, float(GLOBALS::SCALED_CUBE_SIZE)));
                 transform->getHitbox().size.y = GLOBALS::SCALED_CUBE_SIZE;

                 spritesheet->setEntityHeight(GLOBALS::ORIGINAL_CUBE_SIZE);
                 spritesheet->setSpritesheetCoordinates(Map::EnemyIDCoordinates.at(inCloudID));
             }),
             new WaitCommand(0.75f),
             // Move out of the cloud and launch a spine
             new RunCommand([=, this]() {
                 if (!m_world->hasEntity(entity)) {
                     return;
                 }

                 transform->getSize().y = 2;
                 transform->move(sf::Vector2f(0.0f, -float(GLOBALS::SCALED_CUBE_SIZE)));
                 transform->getHitbox().size.y = 2 * GLOBALS::SCALED_CUBE_SIZE;

                 spritesheet->setEntityHeight(2 * GLOBALS::ORIGINAL_CUBE_SIZE);
                 spritesheet->setSpritesheetCoordinates(Map::EnemyIDCoordinates.at(id));

                 createSpine(entity);
             })}));
    };

    lakitu->addComponent<TimerComponent>(throwSpine, 3 * GLOBALS::FPS);
}

void MapSystem::createBuzzyBeetle(int x, int y, int id)
{
    auto entity = m_world->create();
    entity->addComponent<TransformComponent>(
        sf::Vector2f(x * GLOBALS::SCALED_CUBE_SIZE, y * GLOBALS::SCALED_CUBE_SIZE));
    entity->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::EnemySpriteSheet));
    entity->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               1,
                                               1,
                                               0,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               Map::EnemyIDCoordinates.at(id));
    std::vector<int> frameIDs{id, id + 1};
    int framesPerSecond = 6;
    entity->addComponent<AnimationComponent>(frameIDs, framesPerSecond, Map::EnemyIDCoordinates);
    entity->addComponent<MovingComponent>(sf::Vector2f(-GLOBALS::ENEMY_SPEED, 0.0f),
                                          sf::Vector2f(0.0f, 0.0f));
    entity->addComponent<GravityComponent>();

    auto deadID = getReferenceEnemyIDAsEntity(id, 89);
    entity->addComponent<CrushableComponent>([=](Entity *entity) {
        entity->getComponent<EnemyComponent>()->setType(EnemyType::KoopaShell);
        entity->getComponent<MovingComponent>()->getVelocity().x = 0.0f;
        entity->addComponent<DestroyOutsideCameraComponent>();
        entity->getComponent<SpritesheetComponent>()->setSpritesheetCoordinates(
            Map::EnemyIDCoordinates.at(deadID));
        entity->remove<AnimationComponent>();
    });

    entity->addComponent<EnemyComponent>(EnemyType::BuzzyBeetle);
}

void MapSystem::createBulletBillCannon(int x, int y, int id)
{
    auto entity = createBlockEntity(x, y, id);
    auto bulletBillID = 90;
    if (id == 79) // Underground
    {
        bulletBillID = 195;
    } else if (id == 95) // Underwater
    {
        bulletBillID = 300;
    } else if (id == 591) // Castle
    {
        bulletBillID = 405;
    }

    entity->addComponent<TimerComponent>(
        [=](Entity *entity) {
            if (!Camera::GetInstance().inCameraRange(entity->getComponent<TransformComponent>())) {
                return;
            }
            std::cout << "Tutaj" << std::endl;
            auto bulletBill = m_world->create();

            std::random_device rd;
            std::mt19937 gen(rd()); // Mersenne Twister generator
            std::uniform_int_distribution<int> dist(0, 1);

            bool randomDirection = (dist(gen) == 1);
            int intRandomDirection = randomDirection ? 1 : 0;

            bulletBill->addComponent<TransformComponent>(
                sf::Vector2f((x + intRandomDirection * 2 - 1) * GLOBALS::SCALED_CUBE_SIZE,
                             y * GLOBALS::SCALED_CUBE_SIZE),
                sf::Vector2i(1, 1),
                sf::Rect<int>({0, 0}, {GLOBALS::SCALED_CUBE_SIZE, GLOBALS::SCALED_CUBE_SIZE}));
            bulletBill->addComponent<TextureComponent>(
                Game::s_textureHolder->get(TextureID::EnemySpriteSheet));
            bulletBill->getComponent<TextureComponent>()->setHorizontalFlipped(randomDirection);

            bulletBill->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                                           GLOBALS::ORIGINAL_CUBE_SIZE,
                                                           1,
                                                           1,
                                                           0,
                                                           GLOBALS::ORIGINAL_CUBE_SIZE,
                                                           GLOBALS::ORIGINAL_CUBE_SIZE,
                                                           Map::BlockIDCoordinates.at(bulletBillID
                                                                                      + 26));
            auto xVelocity = randomDirection ? 3.0f : -3.0f;
            bulletBill->addComponent<MovingComponent>(sf::Vector2f(xVelocity, 0.0f),
                                                      sf::Vector2f(0.0f, 0.0f));
            bulletBill->addComponent<DestroyOutsideCameraComponent>();
            bulletBill->addComponent<FrictionExemptComponent>();
            bulletBill->addComponent<ParticleComponent>();

            //local cannonSound = Concord.entity(self.world)
            //cannonSound:give('sound_component', SOUND_ID.CANNON_FIRE)
            bulletBill->addComponent<CrushableComponent>([=](Entity *entity) {
                entity->getComponent<MovingComponent>()->getVelocity().x = 0.0f;
                entity->addComponent<DeadComponent>();
                entity->addComponent<GravityComponent>();
            });

            bulletBill->addComponent<EnemyComponent>(EnemyType::BulletBill);
        },
        4 * GLOBALS::FPS);
}

void MapSystem::addWarpPipeComponetToEntity(Entity *entity, LevelData::Warp warp)
{
    auto transform = entity->getComponent<TransformComponent>();
    sf::Vector2i playerCoordinates;
    playerCoordinates.x = warp.teleportCoords.x;
    playerCoordinates.y = warp.teleportCoords.y;

    sf::Vector2i cameraLocation;
    cameraLocation.x = warp.cameraCoords.x;
    cameraLocation.y = warp.cameraCoords.y;

    Direction inDirection = warp.goingIn;
    Direction outDirection = warp.goingOut;
    bool cameraFreeze = warp.cameraFreeze;
    sf::Color color = warp.bgColor;
    LevelType levelType = warp.levelType;
    sf::Vector2i newLevel;
    newLevel.x = warp.levelToGo.x;
    newLevel.y = warp.levelToGo.y;

    entity->addComponent<WarpPipeComponent>(playerCoordinates,
                                            cameraLocation,
                                            inDirection,
                                            outDirection,
                                            cameraFreeze,
                                            color,
                                            levelType,
                                            newLevel);

    if (inDirection == Direction::Up) {
        transform->getHitbox().position.x = 32.0f;
        transform->getHitbox().size.x = 0.0f;
    } else if (inDirection == Direction::Down) {
        transform->getHitbox().position.x = 32.0f;
        transform->getHitbox().size.x = 0.0f;
    } else if (inDirection == Direction::Left) {
        transform->getHitbox().position.y = 32.0f;
        transform->getHitbox().size.y = 0.0f;
    } else if (inDirection == Direction::Right) {
        transform->getHitbox().position.y = 32.0f;
        transform->getHitbox().size.y = 0.0f;
    }
}

void MapSystem::addItemDispenser(Entity *entity, int id)
{
    // To Do
    auto mysteryBox = entity->getComponent<MysteryBoxComponent>();
    auto deactivatedID = getReferenceBlockIDAsEntity(id, 196);
    mysteryBox->setDeactivatedCoordinates(Map::BlockIDCoordinates.at(deactivatedID));

    if (mysteryBox->boxType() == MysteryBoxType::Mushroom) {
        mysteryBox->setWhenDispensed([=](Entity *entity) {
            if (m_world->findFirst<PlayerComponent>()
                    ->getComponent<PlayerComponent>()
                    ->getPlayerState()
                == PlayerState::SmallMario) {
                dispenseMushroom(entity);
            } else {
                dispenseFlower(entity, id);
            }
        });
    } else if (mysteryBox->boxType() == MysteryBoxType::Coins) {
        mysteryBox->setWhenDispensed([=](Entity *entity) { dispenseCoin(entity, id); });
    } else if (mysteryBox->boxType() == MysteryBoxType::SuperStar) {
        mysteryBox->setWhenDispensed([=](Entity *entity) { dispenseStar(entity, id); });
    } else if (mysteryBox->boxType() == MysteryBoxType::OneUp) {
        mysteryBox->setWhenDispensed([=](Entity *entity) { dispenseOneUp(entity, id); });
    } else if (mysteryBox->boxType() == MysteryBoxType::Vines) {
        mysteryBox->setWhenDispensed([=](Entity *entity) {
            auto vineData = getLevelData().getVines();
            if (vineData[0].vineCoords.x == 0 && vineData[0].vineCoords.y == 0) {
                return;
            }
            dispenseVine(entity, id);
        });
    }
}

void MapSystem::dispenseMushroom(Entity *originalBlock)
{
    auto mushroom = m_world->create();
    auto blockTransform = originalBlock->getComponent<TransformComponent>();
    mushroom->addComponent<TransformComponent>(sf::Vector2f{blockTransform->getPosition()});
    mushroom->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::BlockTileSheet));
    mushroom->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                                 GLOBALS::ORIGINAL_CUBE_SIZE,
                                                 1,
                                                 1,
                                                 1,
                                                 GLOBALS::ORIGINAL_CUBE_SIZE,
                                                 GLOBALS::ORIGINAL_CUBE_SIZE,
                                                 Map::BlockIDCoordinates.at(608));
    mushroom->addComponent<CollectibleComponent>(CollectibleType::Mushroom);
    mushroom->addComponent<MovingComponent>(sf::Vector2f{0.0f, -1.0f}, sf::Vector2f{0.0f, 0.0f});
    mushroom->addComponent<CollisionExemptComponent>();
    mushroom->addComponent<WaitUntilComponent>(
        [blockTransform](Entity *entity) {
            return blockTransform->getTop()
                   > entity->getComponent<TransformComponent>()->getBottom();
        },
        [](Entity *entity) {
            entity->addComponent<GravityComponent>();
            entity->getComponent<MovingComponent>()->getVelocity().x = GLOBALS::COLLECTIBLE_SPEED;
            entity->remove<CollisionExemptComponent>();
            entity->remove<WaitUntilComponent>();
        });
}

void MapSystem::dispenseFlower(Entity *originalBlock, int id)
{
    auto fireFlower = m_world->create();
    auto blockTransform = originalBlock->getComponent<TransformComponent>();
    fireFlower->addComponent<TransformComponent>(sf::Vector2f{blockTransform->getPosition()});
    auto flowerID = getReferenceBlockIDAsEntity(id, 48);
    fireFlower->addComponent<TextureComponent>(
        Game::s_textureHolder->get(TextureID::BlockTileSheet));
    fireFlower->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                                   GLOBALS::ORIGINAL_CUBE_SIZE,
                                                   1,
                                                   1,
                                                   1,
                                                   GLOBALS::ORIGINAL_CUBE_SIZE,
                                                   GLOBALS::ORIGINAL_CUBE_SIZE,
                                                   Map::BlockIDCoordinates.at(flowerID));
    const std::vector<int> frameIDs{flowerID, flowerID + 1, flowerID + 2, flowerID + 3};
    const int framesPerSecond = 8;
    fireFlower->addComponent<AnimationComponent>(frameIDs, framesPerSecond, Map::BlockIDCoordinates);
    fireFlower->addComponent<CollectibleComponent>(CollectibleType::FireFlower);
    fireFlower->addComponent<MovingComponent>(sf::Vector2f{0.0f, -1.0f}, sf::Vector2f{0.0f, 0.0f});
    fireFlower->addComponent<CollisionExemptComponent>();
    fireFlower->addComponent<WaitUntilComponent>(
        [blockTransform](Entity *entity) {
            return blockTransform->getTop()
                   > entity->getComponent<TransformComponent>()->getBottom();
        },
        [](Entity *entity) {
            entity->addComponent<GravityComponent>();
            entity->getComponent<MovingComponent>()->getVelocity().y = 0.0f;
            entity->remove<CollisionExemptComponent>();
            entity->remove<WaitUntilComponent>();
        });
}

void MapSystem::dispenseCoin(Entity *originalBlock, int id)
{
    // local coinSound = Concord.entity(self.world)
    // coinSound:give('sound_component', SOUND_ID.COIN)
    auto addScore = m_world->create();
    addScore->addComponent<AddScoreComponent>(100, true);

    auto floatingText = m_world->create();
    floatingText->addComponent<CreateFloatingTextComponent>(originalBlock, std::to_string(100));

    auto coin = m_world->create();
    auto transform = originalBlock->getComponent<TransformComponent>();
    coin->addComponent<TransformComponent>(
        sf::Vector2f{transform->getPosition().x, transform->getPosition().y});
    coin->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::BlockTileSheet));
    coin->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                             GLOBALS::ORIGINAL_CUBE_SIZE,
                                             1,
                                             1,
                                             1,
                                             GLOBALS::ORIGINAL_CUBE_SIZE,
                                             GLOBALS::ORIGINAL_CUBE_SIZE,
                                             Map::BlockIDCoordinates.at(656));
    coin->addComponent<ForegroundComponent>();
    const std::vector<int> frameIDs{656, 657, 658, 659};
    const int framesPerSecond = 8;

    coin->addComponent<AnimationComponent>(frameIDs, framesPerSecond, Map::BlockIDCoordinates);
    coin->addComponent<GravityComponent>();
    coin->addComponent<MovingComponent>(sf::Vector2f{0.0f, -10.0}, sf::Vector2f{0.0f, 0.3f});
    coin->addComponent<ParticleComponent>();
    coin->addComponent<WaitUntilComponent>(
        [transform](Entity *entity) {
            return AABB::isCollision(transform, entity->getComponent<TransformComponent>())
                   && entity->getComponent<MovingComponent>()->getVelocity().y >= 0.0f;
        },
        [this](Entity *entity) { m_world->destroy(entity); });
}

void MapSystem::dispenseStar(Entity *originalBlock, int id)
{
    Entity *dispenseSound(m_world->create());
    dispenseSound->addComponent<SoundComponent>(SoundID::PowerUpAppear);
    auto blockTransform = originalBlock->getComponent<TransformComponent>();

    Entity *star = m_world->create();
    star->addComponent<TransformComponent>(
        sf::Vector2f{blockTransform->getPosition().x, blockTransform->getPosition().y});
    int starID = getReferenceBlockIDAsEntity(id, 96);
    star->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::BlockTileSheet));
    star->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                             GLOBALS::ORIGINAL_CUBE_SIZE,
                                             1,
                                             1,
                                             1,
                                             GLOBALS::ORIGINAL_CUBE_SIZE,
                                             GLOBALS::ORIGINAL_CUBE_SIZE,
                                             Map::BlockIDCoordinates.at(starID));
    const std::vector<int> frameIDs{starID, starID + 1, starID + 2, starID + 3};
    const int framesPerSecond = 8;
    star->addComponent<AnimationComponent>(frameIDs, framesPerSecond, Map::BlockIDCoordinates);
    star->addComponent<CollectibleComponent>(CollectibleType::SuperStar);
    star->addComponent<MovingComponent>(sf::Vector2f{0.0f, -1.0f}, sf::Vector2f{0.0f, 0.0f});
    star->addComponent<CollisionExemptComponent>();
    star->addComponent<WaitUntilComponent>(
        [=](Entity *entity) {
            return blockTransform->getTop()
                   > entity->getComponent<TransformComponent>()->getBottom();
        },
        [&](Entity *entity) {
            entity->addComponent<GravityComponent>();
            entity->getComponent<MovingComponent>()->getVelocity().x = GLOBALS::COLLECTIBLE_SPEED;
            entity->remove<CollisionExemptComponent>();
            entity->remove<WaitUntilComponent>();
        });
}

void MapSystem::dispenseOneUp(Entity *originalBlock, int id)
{
    Entity *dispenseSound(m_world->create());
    dispenseSound->addComponent<SoundComponent>(SoundID::PowerUpAppear);

    auto *blockTransform = originalBlock->getComponent<TransformComponent>();

    Entity *oneup = m_world->create();
    oneup->addComponent<TransformComponent>(sf::Vector2f{blockTransform->getPosition()});
    int oneupID = getReferenceBlockIDAsEntity(id, 52);
    oneup->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::BlockTileSheet));
    oneup->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                              GLOBALS::ORIGINAL_CUBE_SIZE,
                                              1,
                                              1,
                                              1,
                                              GLOBALS::ORIGINAL_CUBE_SIZE,
                                              GLOBALS::ORIGINAL_CUBE_SIZE,
                                              Map::BlockIDCoordinates.at(oneupID));
    oneup->addComponent<CollectibleComponent>(CollectibleType::OneUp);
    oneup->addComponent<MovingComponent>(sf::Vector2f{0.0f, -1.0f}, sf::Vector2f{0.0f, 0.0f});
    oneup->addComponent<CollisionExemptComponent>();
    oneup->addComponent<WaitUntilComponent>(
        [=](Entity *entity) {
            return blockTransform->getTop()
                   > entity->getComponent<TransformComponent>()->getBottom();
        },
        [](Entity *entity) {
            entity->addComponent<GravityComponent>();
            entity->getComponent<MovingComponent>()->getVelocity().x = GLOBALS::COLLECTIBLE_SPEED;
            entity->remove<CollisionExemptComponent>();
            entity->remove<WaitUntilComponent>();
        });
}

void MapSystem::dispenseVine(Entity *originalBlock, int id)
{
    auto vineParts = std::make_shared<std::vector<Entity *>>();
    auto vineLength = std::make_shared<int>(0);
    auto vineData = std::make_shared<LevelData::Vine>(getLevelData().getVines()[0]);

    auto vineTopID = getReferenceBlockIDAsEntity(id, 100);
    auto vineBodyID = getReferenceBlockIDAsEntity(id, 148);

    originalBlock->addComponent<AboveForegroundComponent>();
    auto vineTop = m_world->create();
    auto transform = originalBlock->getComponent<TransformComponent>();
    vineTop->addComponent<TransformComponent>(sf::Vector2f(transform->getPosition()));
    vineTop->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::BlockTileSheet));
    vineTop->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                                GLOBALS::ORIGINAL_CUBE_SIZE,
                                                1,
                                                1,
                                                1,
                                                GLOBALS::ORIGINAL_CUBE_SIZE,
                                                GLOBALS::ORIGINAL_CUBE_SIZE,
                                                Map::BlockIDCoordinates.at(vineTopID));
    vineTop->addComponent<MovingComponent>(sf::Vector2f(0.0f, -1.0f), sf::Vector2f(0.0f, 0.0f));
    vineTop->addComponent<MoveOutsideCameraComponent>();
    vineTop->addComponent<FrictionExemptComponent>();
    vineTop->addComponent<CollisionExemptComponent>();
    vineTop->addComponent<VineComponent>(*vineData, vineParts);
    vineTop->addComponent<ForegroundComponent>();
    (*vineParts).push_back(vineTop);
    ++(*vineLength);
    // local dispenseSound = Concord.entity(self.world)
    // dispenseSound:give('sound_component', SOUND_ID.POWER_UP_APPEAR)
    /*
        Periodically waits until the bottom of the vine has moved past the block, and then
                adds another piece to the vine.
            This keeps happening until the vine has fully grown
    */
    auto vineGrowController = m_world->create();
    vineGrowController->addComponent<WaitUntilComponent>(
        [vineParts, originalBlock](Entity *entity) {
            auto transform = originalBlock->getComponent<TransformComponent>();
            auto lastVinePartsBottom = vineParts->at(vineParts->size() - 1)
                                           ->getComponent<TransformComponent>()
                                           ->getBottom();
            return lastVinePartsBottom <= transform->getTop();
        },
        [vineParts, vineLength, vineData, originalBlock, this, vineBodyID, vineGrowController](
            Entity *entity) {
            auto transform = originalBlock->getComponent<TransformComponent>();
            // Adds another part to the vine
            if (*vineLength < 6) {
                auto vinePiece = m_world->create();
                vinePiece->addComponent<TransformComponent>(transform->getPosition());

                vinePiece->addComponent<TextureComponent>(
                    Game::s_textureHolder->get(TextureID::BlockTileSheet));
                vinePiece->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                                              GLOBALS::ORIGINAL_CUBE_SIZE,
                                                              1,
                                                              1,
                                                              1,
                                                              GLOBALS::ORIGINAL_CUBE_SIZE,
                                                              GLOBALS::ORIGINAL_CUBE_SIZE,
                                                              Map::BlockIDCoordinates.at(
                                                                  vineBodyID));
                vinePiece->addComponent<MovingComponent>(sf::Vector2f(0.0f, -1.0f),
                                                         sf::Vector2f(0.0f, 0.0f));
                vinePiece->addComponent<MoveOutsideCameraComponent>();
                vinePiece->addComponent<CollisionExemptComponent>();
                vinePiece->addComponent<VineComponent>(*vineData, vineParts);
                vinePiece->addComponent<ForegroundComponent>();
                vineParts->push_back(vinePiece);
                (*vineLength)++;
            }
            // If the vine is fully grown and the last vine is no longer in the block
            else if (vineParts->at(vineParts->size() - 1)
                         ->getComponent<TransformComponent>()
                         ->getBottom()
                     < transform->getTop()) {
                for (auto *vinePart : *vineParts) {
                    vinePart->getComponent<MovingComponent>()->getVelocity().y = 0.0f;
                }
                vineGrowController->remove<WaitUntilComponent>();
                m_world->destroy(vineGrowController);
            }
        });
}

Entity *MapSystem::createBlockEntity(int x, int y, int id)
{
    Entity *entity = m_world->create();
    entity->addComponent<TransformComponent>(
        sf::Vector2f(x * GLOBALS::SCALED_CUBE_SIZE, y * GLOBALS::SCALED_CUBE_SIZE));
    entity->addComponent<TextureComponent>(Game::s_textureHolder->get(TextureID::BlockTileSheet));
    entity->addComponent<SpritesheetComponent>(GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               1,
                                               1,
                                               1,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               GLOBALS::ORIGINAL_CUBE_SIZE,
                                               Map::BlockIDCoordinates.at(id));
    entity->addComponent<ForegroundComponent>();
    entity->addComponent<TileComponent>();
    return entity;
}
