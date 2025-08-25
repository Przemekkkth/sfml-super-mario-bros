#include "AnimationSystem.h"
#include "../World.h"
#include "../components/AnimationComponent.h"
#include "../components/EndingBlinkComponent.h"
#include "../components/TextureComponent.h"
#include "../components/TransformComponent.h"
#include "../components/SpritesheetComponent.h"
#include "../components/IconComponent.h"
#include "../components/PausedAnimationComponent.h"
#include "../../Camera.h"


void AnimationSystem::tick(World *world)
{
    handleEndingBlink(world);
    handleAnimation(world);
    handlePausedAnimation(world);
}

void AnimationSystem::handleEndingBlink(World *world)
{
    world->find<EndingBlinkComponent, TextureComponent>([](Entity* entity) {
        auto blink = entity->getComponent<EndingBlinkComponent>();
        blink->increase();
        blink->decreaseTime();

        if ((blink->getBlinkProgress()) % 2 == 1)
        {
            entity->getComponent<TextureComponent>()->setVisible(false);
        }
        else
        {
            entity->getComponent<TextureComponent>()->setVisible(true);
        }

        if (blink->isFinished())
        {
            entity->remove<EndingBlinkComponent>();
            entity->getComponent<TextureComponent>()->setVisible(true);
        }
    });
}

void AnimationSystem::handleAnimation(World *world)
{
    world->find<AnimationComponent, TextureComponent, SpritesheetComponent, TransformComponent>(
        [](Entity* entity) {
            if ((!Camera::GetInstance().inCameraRange(entity->getComponent<TransformComponent>()) &&
                 !entity->hasComponent<IconComponent>()) ||
                entity->hasComponent<PausedAnimationComponent>())
            {
                return;
            }

            auto* animation = entity->getComponent<AnimationComponent>();
            auto* spritesheet = entity->getComponent<SpritesheetComponent>();

            animation->decreaseFrameTimer();

            if (animation->isFrameFinished() && animation->isPlaying())
            {
                animation->resetTimer();
                animation->increaseFrame();
                if (animation->isAnimationFinished())
                {
                    if (animation->isRepeated())
                    {
                        animation->setCurrentFrame(0);
                    }
                    else
                    {
                        entity->remove<AnimationComponent>();
                        return;
                    }
                }

                // Sets the texture sprite sheets coordinates to the animation frame coordinates
                spritesheet->setSpritesheetCoordinates(animation->getFrameCoordinates());
            }
        });

}

void AnimationSystem::handlePausedAnimation(World *world)
{
    world->find<AnimationComponent, PausedAnimationComponent, TextureComponent, SpritesheetComponent,
                TransformComponent>([](Entity* entity) {
        if (!Camera::GetInstance().inCameraRange(entity->getComponent<TransformComponent>()) &&
            !entity->hasComponent<IconComponent>()) {
            return;
        }
        auto* animation = entity->getComponent<AnimationComponent>();
        auto* spritesheet = entity->getComponent<SpritesheetComponent>();
        auto* pause = entity->getComponent<PausedAnimationComponent>();

        // If it is playing then it increases the frame, and it also checks if it should pause
        if (animation->isPlaying())
        {
            animation->decreaseFrameTimer();

            if (animation->isFrameFinished())
            {
                animation->resetTimer();
                animation->increaseFrame();

                if (animation->isAnimationFinished())
                {
                    if (animation->isRepeated())
                    {
                        animation->setCurrentFrame(0);
                    }
                    else
                    {
                        // Sets the texture sprite sheets coordinates to the animation frame
                        // coordinates
                        spritesheet->setSpritesheetCoordinates(animation->getFrameCoordinates());
                        entity->remove<AnimationComponent>();
                        return;
                    }
                }
                if (animation->getCurrentFrame() == pause->getPauseFrame())
                {
                    pause->pause(pause->getPauseLength());
                    animation->setPlaying(false);
                }
            }
        }
        else
        {
            pause->decreaseTimer();

            if (pause->isFinished()) {
                animation->setPlaying(true);
            }
        }

        // Sets the texture sprite sheets coordinates to the animation frame coordinates
        spritesheet->setSpritesheetCoordinates(animation->getFrameCoordinates());
    });
}
