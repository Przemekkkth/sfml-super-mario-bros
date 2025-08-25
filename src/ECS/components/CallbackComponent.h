#ifndef CALLBACKCOMPONENT_H
#define CALLBACKCOMPONENT_H
#include "Component.h"
#include <functional>
class Entity;
class CallbackComponent : public Component
{
public:
    CallbackComponent(std::function<void(Entity *)> callback, int time)
        : m_callback{callback}
        , m_time{time}
    {}
    void setTime(int t)
    {
        m_time = t;
    }
    int getTime() const
    {
        return m_time;
    }
    void setCallback(std::function<void(Entity*)>& c)
    {
        m_callback = c;
    }
    std::function<void(Entity *)> getCallback() { return m_callback; }

private:
    std::function<void(Entity*)> m_callback;
    int m_time;
};

/*
struct CallbackComponent : public Component {
    CallbackComponent() = default;
    CallbackComponent(std::function<void(Entity*)> callback, int time)
        : callback{callback}, time{time} {}

    std::function<void(Entity*)> callback;
    int time;
};
*/
#endif // CALLBACKCOMPONENT_H
