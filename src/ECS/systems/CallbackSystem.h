#ifndef CALLBACKSYSTEM_H
#define CALLBACKSYSTEM_H
#include "System.h"

class CallbackSystem : public System
{
public:
    CallbackSystem();
    virtual void tick(World *world) override;
};

#endif // CALLBACKSYSTEM_H
