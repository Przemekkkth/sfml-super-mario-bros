#ifndef LAKITUCOMPONENT_H
#define LAKITUCOMPONENT_H
#include "../../utils/PIDController.h"
#include "../../utils/ResourceIdentifiers.h"
#include "Component.h"

class LakituComponent : public Component
{
public:
    int &getSideChangeTimer() { return m_sideChangeTimer; }
    Direction &getLakituSide() { return m_lakituSide; }
    PIDController &getSpeedController() { return m_speedController; }

private:
    int m_sideChangeTimer = 0;
    Direction m_lakituSide = Direction::Left;
    PIDController m_speedController = PIDController(0.04, 0.0, 0.0);
};

#endif // LAKITUCOMPONENT_H
