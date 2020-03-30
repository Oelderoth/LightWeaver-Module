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
                currentlyRunningAnimations++;

                if (context.state == AnimationState::Started) {
                    context.callback(AnimationParam(0.0f, context.animation.easingFunction(0.0f), context.iterations, AnimationState::Started));
                    context.state = AnimationState::Running;
                    continue;
                }

                if (context.remainingDuration <= delta) {
                    if (context.animation.loop) {
                        context.callback(AnimationParam(1.0f, context.animation.easingFunction(1.0f), context.iterations, AnimationState::Running));
                        context.loop();
                        continue;
                    } else {
                        context.callback(AnimationParam(1.0f, context.animation.easingFunction(1.0f), context.iterations, AnimationState::Completed));
                        context.stop();
                        continue;
                    }
                }

                float progress = (float)(context.animation.duration - context.remainingDuration) / (float)(context.animation.duration);
                context.remainingDuration -= delta;

                context.callback(AnimationParam(progress, context.animation.easingFunction(progress), context.iterations, context.state));

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

    uint16_t Animator::playAnimation(uint16_t index, const Animation& animation) const {
        if (index >= maxAnimations) {
            return 0xFFFF;
        }

        animations[index].stop();

        uint16_t uid = (uint16_t) random(0xFFFF);
        animations[index].start(uid, animation);
            
        return index;
    }

    uint16_t Animator::playAnimation(uint16_t index, const Animation* animation) const {
        if (!animation) return 0xFFFF;
        return playAnimation(index, *animation);
    }

    uint16_t Animator::playAnimation(const Animation& animation) const {
        for (uint16_t i = 0; i < maxAnimations; i++) {
            if (!animations[i].isActive()) {
                return playAnimation(i, animation);
            }
        }
        return playAnimation(maxAnimations-1, animation);
    }

    uint16_t Animator::playAnimation(const Animation* animation) const {
        if (!animation) return 0xFFFF;
        return playAnimation(*animation);
    }

    void Animator::stopAnimation(uint16_t animation) const {
        if (animation < maxAnimations) {
            animations[animation].stop();
        }
    }

    void Animator::pauseAnimation(uint16_t animation) const {
        if (animation < maxAnimations) {
            animations[animation].pause();
        }
    }

    void Animator::resumeAnimation(uint16_t animation) const {
        if (animation < maxAnimations) {
            animations[animation].resume();
        }
    }
}