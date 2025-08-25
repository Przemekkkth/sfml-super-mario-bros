#ifndef PRINTCOMMAND_H
#define PRINTCOMMAND_H

#include "Command.h"
#include <iostream>
#include <string>

class PrintCommand : public Command {
public:
    PrintCommand(std::string message) : message{message}
    {}

    virtual void execute() final
    {
        std::cout << message << '\n';
    }

private:
    std::string message;
};

#endif // PRINTCOMMAND_H
