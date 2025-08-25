#ifndef MAP_H
#define MAP_H
#include <vector>
#include <unordered_map>
#include <SFML/System/Vector2.hpp>

class Map
{
public:
    Map();
    Map(const char* dataPath);

    ~Map() = default;

    void loadMap(const char* dataPath);

    void reset();

    static void loadBlockIDS();
    static void loadPlayerIDS();
    static void loadEnemyIDS();
    static void loadIrregularBlockReferences();

    std::vector<std::vector<int>> getLevelData();

    static std::unordered_map<int, sf::Vector2i> BlockIDCoordinates;
    static std::unordered_map<int, sf::Vector2i> PlayerIDCoordinates;
    static std::unordered_map<int, sf::Vector2i> EnemyIDCoordinates;
    static std::unordered_map<int, int> DeadEnemyIDCoordinates;
    static std::unordered_map<int, int> IrregularBlockReferences;

private:
    std::vector<std::vector<int>> m_levelData;
};

#endif // MAP_H
