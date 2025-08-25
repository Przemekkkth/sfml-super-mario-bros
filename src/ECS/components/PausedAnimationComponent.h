#ifndef PAUSEDANIMATIONCOMPONENT_H
#define PAUSEDANIMATIONCOMPONENT_H
#include "Component.h"

class PausedAnimationComponent : public Component
{
public:
    PausedAnimationComponent(int pauseFrame, int pauseLength)
        : m_pauseFrame{pauseFrame}, m_pauseLength{pauseLength}
    {}

    void pause(int pauseLength)
    {
        m_pauseTimer = pauseLength;
    }

    int getPauseLength() const
    {
        return m_pauseLength;
    }

    int getPauseTimer() const
    {
        return m_pauseTimer;
    }

    int getPauseFrame() const
    {
        return m_pauseFrame;
    }

    void decreaseTimer()
    {
        --m_pauseTimer;
    }

    bool isFinished() const
    {
        return m_pauseTimer < 0;
    }
private:
    int m_pauseFrame;
    int m_pauseLength;
    int m_pauseTimer;
};

#endif // PAUSEDANIMATIONCOMPONENT_H
