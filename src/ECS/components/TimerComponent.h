#ifndef TIMERCOMPONENT_H
#define TIMERCOMPONENT_H
#include "Component.h"
#include <functional>

class Entity;
class TimerComponent : public Component
{
public:
    TimerComponent(std::function<void (Entity *)> onExecute, int delay)
        : m_onExecute(onExecute), m_delay{delay}, m_time{delay}
    {
    }

    void reset()
    {
        m_time = m_delay;
    }

    void execute(Entity *entity) { m_onExecute(entity); }

    void setTime(int newTime) { m_time = newTime; }

    int getTime() const { return m_time; }

private:
    std::function<void(Entity*)> m_onExecute;
    int m_delay;
    int m_time;
};

#endif // TIMERCOMPONENT_H
