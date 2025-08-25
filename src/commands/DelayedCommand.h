#ifndef DELAYEDCOMMAND_H
#define DELAYEDCOMMAND_H
#include "Command.h"
#include "../Globals.h"
#include <functional>

class DelayedCommand : public Command
{
public:
    DelayedCommand(std::function<void()> onExecute, float delay)
        : m_onExecute{onExecute}
    {
        if (delay >= 0)
        {
            m_ticks = static_cast<int>(delay * GLOBALS::FPS);
        }
        else {
            m_ticks = 0;
        }
    }

    virtual void execute() final
    {
        --m_ticks;
        if (m_ticks == 0)
        {
            m_onExecute();
        }
    }

    virtual bool isFinished() final
    {
        return m_ticks <= 0;
    }

private:
    std::function<void()> m_onExecute;
    int m_ticks;
};

#endif // DELAYEDCOMMAND_H
