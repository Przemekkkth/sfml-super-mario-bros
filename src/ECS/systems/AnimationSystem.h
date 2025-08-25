#ifndef ANIMATIONSYSTEM_H
#define ANIMATIONSYSTEM_H
#include "System.h"

class AnimationSystem : public System
{
public:
    AnimationSystem() = default;
    ~AnimationSystem() = default;
    virtual void tick(World *world) override;

private:
    void handleEndingBlink(World *world);
    void handleAnimation(World* world);
    void handlePausedAnimation(World* world);
};

#endif // ANIMATIONSYSTEM_H
