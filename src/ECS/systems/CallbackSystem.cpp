#include "CallbackSystem.h"
#include "../World.h"
#include "../components/CallbackComponent.h"
#include "../components/DestroyDelayedComponent.h"
#include "../components/TimerComponent.h"
#include "../components/WaitUntilComponent.h"

CallbackSystem::CallbackSystem() {}

void CallbackSystem::tick(World *world)
{
    world->find<CallbackComponent>([](Entity *entity) {
        auto *callback = entity->getComponent<CallbackComponent>();
        callback->setTime(callback->getTime() - 1);

        if (callback->getTime() == 0) {
            callback->getCallback()(entity);
            //callback->callback(entity);
            entity->remove<CallbackComponent>();
        }
    });

    world->find<WaitUntilComponent>([](Entity *entity) {
        auto *waitUntil = entity->getComponent<WaitUntilComponent>();

        if (waitUntil->getConditionFunc()(entity)) {
            waitUntil->getDoAfterFunc()(entity);
        }
    });

    world->find<TimerComponent>([](Entity *entity) {
        auto *timer = entity->getComponent<TimerComponent>();

        timer->setTime(timer->getTime() - 1);
        if (timer->getTime() == 0) {
            timer->execute(entity);
            timer->reset();
        }
    });

    world->find<DestroyDelayedComponent>([world](Entity *entity) {
        auto *destroy = entity->getComponent<DestroyDelayedComponent>();

        if (destroy->getTime() > 0) {
            destroy->setTime(destroy->getTime() - 1);
        } else if (destroy->getTime() <= 0) {
            world->destroy(entity);
        }
    });
}
