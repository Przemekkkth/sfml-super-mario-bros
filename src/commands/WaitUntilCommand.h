#ifndef WAITUNTILCOMMAND_H
#define WAITUNTILCOMMAND_H
#include "Command.h"
#include <functional>
class WaitUntilCommand : public Command
{
public:
    WaitUntilCommand(std::function<bool()> condition)
        : m_condition{condition}
    {}

    virtual bool isFinished() override
    {
        return m_condition();
    }
private:
    std::function<bool()> m_condition;
};

#endif // WAITUNTILCOMMAND_H

