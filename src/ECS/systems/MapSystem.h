#ifndef MAPSYSTEM_H
#define MAPSYSTEM_H
#include "System.h"
#include "../../Map.h"
#include "../../LevelData.h"

class World;
class Entity;

class MapSystem : public System
{
public:
    MapSystem();
    ~MapSystem() = default;
    virtual void onAddedToWorld(World *world) override;
    virtual void tick(World *world) override;
    LevelData &getLevelData();

private:
    static int INVALID_CODE;
    void loadLevel();
    void loadEntities();
    void createBackgroundEntities();
    void createUndergroundEntities();
    void createPlatformLevelEntities();
    void createForegroundEntities();
    void createFireBarEntities();
    void createEnemyEntities();
    void createAboveForegroundEntities();
    void createFloatingTextEntities();

    Entity *createBackgroundEntity(int x, int y, int id);
    void createForegroundEntity(int x, int y, int id, int referenceID);
    void createAboveForegroundEntity(int x, int y, int id);
    void createInvsibleBlock(int x, int y, int referenceID);
    void createQuestionBlock(int x, int y, int referenceID);
    void createAxe(int x, int y, int id);
    void createBridgeChain(int x, int y, int id);
    void createCloudPlatform(int x, int y, int id);
    void createBridge(int x, int y, int id);

    int getReferenceBlockID(int entityID);
    int getReferenceBlockIDAsEntity(int entityID, int referenceID);
    int getReferenceEnemyIDAsEntity(int entityID, int referenceID);
    int getReferenceEnemyID(int id);

    void createCoin(int x, int y, int id);
    void createFlagPole(int x, int y, int id);
    void createFlag(int x, int y, int id);
    void createPlatformEntity(
        int x, int y, int id, int length, LevelData::MovingPlatform movingPlatform);
    void create2WideMovingPlatform(int x, int y, int id);
    void create3WideMovingPlatform(int x, int y, int id);
    void createPlatformLevel(LevelData::PlatformLevel platformLevel);

    void createEnemyEntity(int x, int y, int entityID, int referenceID);
    void createKoopa(int x, int y, int id);
    void createKoopaParatroopa(int x, int y, int id);
    void createShiftedKoopa(int x, int y, int id);
    void createGoomba(int x, int y, int id);
    void createShiftedGoomba(int x, int y, int id);
    void createBowser(int x, int y, int id);
    void createPirhannaPlant(int x, int y, int id);
    void createBlooper(int x, int y, int id);
    void createCheepCheep(int x, int y, int id);
    void createRedCheepCheep(int x, int y, int id);
    void createLavaBubble(int x, int y, int id);
    void createHammerBro(int x, int y, int id);
    void createLakitu(int x, int y, int id);
    void createBuzzyBeetle(int x, int y, int id);
    void createBulletBillCannon(int x, int y, int id);

    void addWarpPipeComponetToEntity(Entity *entity, LevelData::Warp warp);

    void addItemDispenser(Entity *entity, int id);
    void dispenseMushroom(Entity *originalBlock);
    void dispenseFlower(Entity *originalBlock, int id);
    void dispenseCoin(Entity *originalBlock, int id);
    void dispenseStar(Entity *originalBlock, int id);
    void dispenseOneUp(Entity *originalBlock, int id);
    void dispenseVine(Entity *originalBlock, int id);

    Entity *createBlockEntity(int x, int y, int id);
    Map m_backgroundMap, m_undergroundMap, m_foregroundMap, m_collectiblesMap, m_enemyMap,
        m_aboveForegroundMap;
    World* m_world;
    LevelData m_levelData;
};

#endif // MAPSYSTEM_H
