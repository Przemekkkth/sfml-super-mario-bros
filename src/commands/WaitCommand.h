#ifndef WAITCOMMAND_H
#define WAITCOMMAND_H
#include "Command.h"
#include "../Globals.h"

class WaitCommand : public Command
{
public:
    WaitCommand(float seconds)
    {
        if (seconds > 0.0f)
        {
            m_ticks = static_cast<int>(seconds * GLOBALS::FPS);
        }
        else
        {
            m_ticks = 0;
        }
    }

    virtual void execute() override
    {
        --m_ticks;
    }
    virtual bool isFinished() override
    {
        return m_ticks <= 0;
    }
private:
    int m_ticks;
};

#endif // WAITCOMMAND_H
