#ifndef DESTROYDELAYEDCOMPONENT_H
#define DESTROYDELAYEDCOMPONENT_H
#include "Component.h"

class DestroyDelayedComponent : public Component
{
public:
    DestroyDelayedComponent(int time)
    : m_time{time}
    {}

    void setTime(int newTime) { m_time = newTime; }

    int getTime() const { return m_time; }

private:
    int m_time;
};

#endif // DESTROYDELAYEDCOMPONENT_H
