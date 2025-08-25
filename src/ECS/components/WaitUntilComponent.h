#ifndef WAITUNTILCOMPONENT_H
#define WAITUNTILCOMPONENT_H
#include "Component.h"
class Entity;
struct WaitUntilComponent : public Component
{
public:
    WaitUntilComponent() = default;
    WaitUntilComponent(std::function<bool(Entity*)> condition, std::function<void(Entity*)> doAfter)
        : m_condition{condition}, m_doAfter{doAfter} {}
    std::function<bool(Entity *)> getConditionFunc() const { return m_condition; }
    std::function<void(Entity *)> getDoAfterFunc() const { return m_doAfter; }

private:
    std::function<bool(Entity*)> m_condition;
    std::function<void(Entity*)> m_doAfter;
};


#endif // WAITUNTILCOMPONENT_H
