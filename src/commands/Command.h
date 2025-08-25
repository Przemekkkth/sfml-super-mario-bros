#ifndef COMMAND_H
#define COMMAND_H

class Command
{
public:
    Command() = default;
    virtual ~Command();

    virtual void execute();
    virtual bool isFinished();
private:
    Command(const Command&) = delete;
    Command& operator=(const Command&) = delete;
};

#endif // COMMAND_H
