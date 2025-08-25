#ifndef SEQUENCECOMMAND_H
#define SEQUENCECOMMAND_H
#include "Command.h"
#include <vector>

class SequenceCommand : public Command
{
public:
    SequenceCommand()
        : m_sequenceSize{0}, m_currentIndex{0}, m_sequenceFinished{false}
    {}

    SequenceCommand(std::vector<Command*> commandList)
        : m_sequenceSize{0}, m_currentIndex{0}, m_sequenceFinished{false}
    {
        addCommands(commandList);
    }

    void addCommands(std::vector<Command *> commandList)
    {
        for (Command* command : commandList)
        {
            m_commandSequence.emplace_back(std::move(command));
        }
        m_sequenceSize += commandList.size();
    }

    virtual void execute() override
    {
        m_commandSequence[m_currentIndex]->execute();
        if (!m_commandSequence[m_currentIndex]->isFinished())
        {
            return;
        }

        delete m_commandSequence[m_currentIndex];
        ++m_currentIndex;

        if (m_currentIndex == m_sequenceSize)
        {
            m_sequenceFinished = true;
        }
    }

    virtual bool isFinished() override
    {
        return m_sequenceFinished;
    }
private:
    std::vector<Command*> m_commandSequence;
    int m_sequenceSize;
    int m_currentIndex;
    bool m_sequenceFinished;
};

#endif // SEQUENCECOMMAND_H
