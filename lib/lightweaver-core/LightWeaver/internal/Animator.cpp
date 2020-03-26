#include "Animator.h"
using namespace LightWeaver;

void Animator::AnimationContext::start(uint16_t duration, bool looping, Animator::AnimationCallback callback, EasingFunction easingFunction) {
    this->duration = duration;
    this->remainingDuration = duration;
    this->callback = callback;
    this->state = AnimationState::Started;
    this->easingFunction = easingFunction;
    this->looping = looping;
}

void Animator::AnimationContext::stop() {
    this->remainingDuration = 0;
    this->state = AnimationState::Stopped;
}

float Animator::AnimationContext::progress() {
    return (float)(duration - remainingDuration) / (float)(duration);
}

bool Animator::AnimationContext::isActive() {
    return (state == AnimationState::Started || state == AnimationState::Running || state == AnimationState::Paused);
}

bool Animator::AnimationContext::isRunning() {
    return (state == AnimationState::Started || state == AnimationState::Running);
}

bool Animator::isAnimating() {
    return currentlyRunning > 0;
}

void Animator::pause() {
    for (uint16_t i = 0; i < animationCount; i++) {
        pauseAnimation(i);
    }
}

void Animator::resume() {
    for (uint16_t i = 0; i < animationCount; i++) {
        resumeAnimation(i);
    }
}

void Animator::stop() {
    for (uint16_t i = 0; i < animationCount; i++) {
        stopAnimation(i);
    }
}

uint16_t Animator::startAnimation(uint16_t index, uint16_t duration, bool looping, Animator::AnimationCallback callback, EasingFunction easingFunction) {
    if (index >= animationCount) {
        index = animationCount-1;
    }

    // Cleanup any existing animation
    stopAnimation(index);
    animations[index].start(duration, looping, callback, easingFunction);

    return index;
}

uint16_t Animator::startAnimation(uint16_t duration, bool looping, Animator::AnimationCallback callback, EasingFunction easingFunction) {
    for (uint16_t i = 0; i < animationCount; i++) {
        if (!animations[i].isActive())
        {
            return startAnimation(i, duration, looping, callback, easingFunction);
        }
    }
    return startAnimation(animationCount-1, duration, looping, callback, easingFunction);
}

void Animator::stopAnimation(uint16_t index) {
    animations[index].stop();
}

void Animator::pauseAnimation(uint16_t index) {
    AnimationContext& context = animations[index];
    if (context.state == AnimationState::Running) {
        context.state = AnimationState::Paused;
    }
}

void Animator::resumeAnimation(uint16_t index) {
    AnimationContext& context = animations[index];
    if (context.state == AnimationState::Paused) {
        context.state = AnimationState::Running;
    }
}

void Animator::loop()  {
    unsigned long tick = millis();
    unsigned long delta = tick - previousTick;
    if (delta > timescale) {
        previousTick = tick;
        delta /= timescale;
        currentlyRunning = 0;
        for (uint16_t i = 0; i < animationCount; i++) {
            AnimationContext& context = animations[i];
            AnimationParam param;
            param.index = i;
            
            if (!context.isRunning()) continue;

            if (context.remainingDuration > delta) { // Animation is running
                param.state = context.state;
                param.progress = context.progress();
                param.easedProgress = context.easingFunction ? context.easingFunction(param.progress) : param.progress;
                context.remainingDuration -= delta;
                if (context.state == AnimationState::Started) {
                    context.state = AnimationState::Running;
                }
            } else if (context.remainingDuration > 0) { // Animation is over
                param.progress = 1.0f;
                param.easedProgress = 1.0f;
                if (context.looping) {
                    context.remainingDuration = context.duration;
                } else {
                    context.remainingDuration = 0;
                    context.state = AnimationState::Completed;
                }
                param.state = context.state;
            }
            if (context.callback) {
                context.callback(param);
            }
        }
    }
}