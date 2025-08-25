#include "GameData.h"

int GameData::LEVEL = 1;
int GameData::SUB_LEVEL = 1;
int GameData::GAME_TIME = 400;
int GameData::LIVES = 4;
int GameData::COINS = 0;
int GameData::SCORE = 0;

void GameData::resetValues()
{
    LIVES = 4;
    COINS = 0;
    SCORE = 0;
}
