#ifndef BOWSERCOMPONENT_H
#define BOWSERCOMPONENT_H
#include "Component.h"
#include "../../utils/ResourceIdentifiers.h"
#include <vector>
#include <functional>

class Entity;
class BowserComponent : public Component
{
public:
    BowserComponent(std::vector<std::function<void(Entity*, int number)>>& attacks,
                    std::vector<std::function<void(Entity*)>>& movements)
        : m_bowserAttacks(attacks), m_bowserMovements(movements)
    {}

    float &getDistanceMoved() { return m_distanceMoved; }
    int &getLastAttackTime() { return m_lastAttackTime; }
    int &getLastMoveTime() { return m_lastMoveTime; }
    int &getlastStopTime() { return m_lastStopTime; }
    int &getLastJumpTime() { return m_lastJumpTime; }
    int &getCurrentMoveIndex() { return m_currentMoveIndex; }

    Direction &getLastMoveDirection() { return m_lastMoveDirection; }
    std::vector<std::function<void(Entity *, int number)>> m_bowserAttacks;
    std::vector<std::function<void(Entity *)>> m_bowserMovements;

private:
    float m_distanceMoved = 0;

    int m_lastAttackTime = 0;
    int m_lastMoveTime = 0;
    int m_lastStopTime = 0;
    int m_lastJumpTime = 0;

    int m_currentMoveIndex = 0;
    Direction m_lastMoveDirection;
};

#endif // BOWSERCOMPONENT_H

/*
struct BowserComponent : public Component {
   BowserComponent(std::vector<std::function<void(Entity*, int number)>> attacks,
                   std::vector<std::function<void(Entity*)>> movements)
       : bowserAttacks{attacks}, bowserMovements{movements} {}

   std::vector<std::function<void(Entity*, int number)>> bowserAttacks;
   std::vector<std::function<void(Entity*)>> bowserMovements;

   float distanceMoved = 0;

   int lastAttackTime = 0;
   int lastMoveTime = 0;
   int lastStopTime = 0;
   int lastJumpTime = 0;

   int currentMoveIndex = 0;

   Direction lastMoveDirection = Direction::NONE;
};

*/
