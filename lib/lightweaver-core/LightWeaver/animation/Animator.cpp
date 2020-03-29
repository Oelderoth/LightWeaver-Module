#include "Animator.h"

namespace LightWeaver {
    void Animator::setup() {}
    void Animator::loop() {
        unsigned long delta = millis() - previousTick;
        if (delta >= timescale) {
            previousTick = millis();
            delta /= timescale;
            currentlyRunningAnimations = 0;

            for (uint16_t i = 0; i < maxAnimations; i++) {
                Animator::AnimationContext& context = animations[i];
                if (!context.isRunning()) continue;

                if (context.remainingDuration <= delta) {
                    if (context.animation.loop) {
                        context.callback(AnimationParam(1.0f, context.animation.easingFunction(1.0f), AnimationState::Running));
                        context.remainingDuration = context.animation.duration;
                        continue;
                    } else {
                        context.callback(AnimationParam(1.0f, context.animation.easingFunction(1.0f), AnimationState::Completed));
                        context.stop();
                        continue;
                    }
                }

                if (context.state == AnimationState::Started) {
                    context.callback(AnimationParam(0.0f, context.animation.easingFunction(0.0f), AnimationState::Started));
                    context.state = AnimationState::Running;
                    continue;
                }

                float progress = (float)(context.animation.duration - context.remainingDuration) / (float)(context.animation.duration);
                context.remainingDuration -= delta;

                context.callback(AnimationParam(progress, context.animation.easingFunction(progress), context.state));

            }
        }
    }

    bool Animator::isAnimating() {
        return currentlyRunningAnimations > 0;
    }

    void Animator::pause() {
        for (uint16_t i = 0; i < maxAnimations; i++) {
            animations[i].pause();
        }
    }

    void Animator::resume() {
        for (uint16_t i = 0; i < maxAnimations; i++) {
            animations[i].resume();
        }
    }

    void Animator::stop() {
        for (uint16_t i = 0; i < maxAnimations; i++) {
            animations[i].stop();
        }
    }

    Animator::AnimationHandle* Animator::playAnimation(uint16_t index, Animation& animation) const {
        if (index >= maxAnimations) {
            index = maxAnimations - 1;
        }

        uint16_t uid = (uint16_t) random(0xFFFF+1);
        animations[index].start(uid, animation, AnimationState::Started);
            
        return new AnimationHandle(uid, index);
    }

    Animator::AnimationHandle* Animator::playAnimation(Animation& animation) const {
        for (uint16_t i = 0; i < maxAnimations; i++) {
            if (!animations[i].isActive()) {
                return playAnimation(i, animation);
            }
        }
        return playAnimation(maxAnimations - 1, animation);
    }

    void Animator::stopAnimation(Animator::AnimationHandle& animation) const {
        if (animation.uid == animations[animation.animationIndex].uid) {
            animations[animation.animationIndex].stop();
        }
    }

    void Animator::pauseAnimation(Animator::AnimationHandle& animation) const {
        if (animation.uid == animations[animation.animationIndex].uid) {
            animations[animation.animationIndex].pause();
        }
    }

    void Animator::resumeAnimation(Animator::AnimationHandle& animation) const {
        if (animation.uid == animations[animation.animationIndex].uid) {
            animations[animation.animationIndex].resume();
        }
    }
}