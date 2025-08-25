#ifndef COLLECTIBLESYSTEM_H
#define COLLECTIBLESYSTEM_H
#include "System.h"

class CollectibleSystem : public System
{
public:
    CollectibleSystem();
    virtual void tick(World *world) override;
};

#endif // COLLECTIBLESYSTEM_H
