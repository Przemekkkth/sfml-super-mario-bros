#ifndef ENDINGBLINKCOMPONENT_H
#define ENDINGBLINKCOMPONENT_H
#include "Component.h"

class EndingBlinkComponent : public Component
{
public:
    EndingBlinkComponent(int blinkSpeed, int time)
        : m_blinkSpeed{blinkSpeed}, m_time{time}
    {}

    int getBlinkProgress() const
    {
        return m_current / m_blinkSpeed;
    }

    void increase()
    {
        ++m_current;
    }

    void decreaseTime()
    {
        --m_time;
    }

    bool isFinished()
    {
        return m_time <= 0;
    }
private:
    const int m_blinkSpeed;
    int m_current = 0;
    int m_time;
};

#endif // ENDINGBLINKCOMPONENT_H
