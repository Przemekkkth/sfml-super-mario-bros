#ifndef RUNCOMMAND_H
#define RUNCOMMAND_H
#include "Command.h"
#include <functional>

class RunCommand : public Command
{
public:
    RunCommand(std::function<void()> execute)
        : m_onExecute{execute}, m_finishedSupplier{[](){return true;}}
    {}

    RunCommand(std::function<void ()> execute, std::function<bool ()> finished)
        : m_onExecute{execute}, m_finishedSupplier{finished}
    {}

    virtual void execute() final
    {
        m_onExecute();
    }

    virtual bool isFinished() final
    {
        return m_finishedSupplier();
    }
private:
    std::function<void()> m_onExecute;
    std::function<bool()> m_finishedSupplier;
};

#endif // RUNCOMMAND_H
