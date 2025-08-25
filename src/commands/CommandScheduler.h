#ifndef COMMANDSCHEDULER_H
#define COMMANDSCHEDULER_H
#include <vector>

class Command;

class CommandScheduler
{
public:
    static CommandScheduler& getInstance();
    CommandScheduler() = default;
    ~CommandScheduler() = default;

    void addCommand(Command* command);
    void run();

private:
    static CommandScheduler instance;

    CommandScheduler(const CommandScheduler&) = delete;
    CommandScheduler& operator=(const CommandScheduler&) = delete;

    void emptyDestroyQueue();

    std::vector<Command*> commandQueue;
    std::vector<Command*> destroyQueue;
};



#endif // COMMANDSCHEDULER_H
