#include "Animator.h"
using namespace LightWeaver;

template <typename T> void Animator<T>::AnimationContext::start(T startValue, T endValue, uint16_t duration, Animator<T>::AnimationCallback callback, EasingFunction easingFunction) {
    this->startValue = startValue;
    this->endValue = endValue;
    this->duration = duration;
    this->remainingDuration = duration;
    this->callback = callback;
    this->state = AnimationState::Started;
    this->easingFunction = easingFunction;
}

template <typename T> void Animator<T>::AnimationContext::stop() {
    this->remainingDuration = 0;
    this->state = AnimationState::Stopped;
}

template <typename T> float Animator<T>::AnimationContext::progress() {
    return (float)(duration - remainingDuration) / (float)(duration);
}

template <typename T> bool Animator<T>::AnimationContext::isActive() {
    return (state == AnimationState::Started || state == AnimationState::Running);
}

template <typename T> bool Animator<T>::isAnimating() {
    return currentlyRunning > 0;
}

template <typename T> void Animator<T>::pause() {
    for (uint16_t i = 0; i < animationCount; i++) {
        pauseAnimation(i);
    }
}

template <typename T> void Animator<T>::resume() {
    for (uint16_t i = 0; i < animationCount; i++) {
        resumeAnimation(i);
    }
}

template <typename T> void Animator<T>::stop() {
    for (uint16_t i = 0; i < animationCount; i++) {
        stopAnimation(i);
    }
}

template <typename T> void Animator<T>::startAnimation(uint16_t index, T startValue, T endValue, uint16_t duration, Animator<T>::AnimationCallback callback, EasingFunction easingFunction) {
    if (index >= animationCount) return;

    // Cleanup any existing animation
    stopAnimation(index);
    animations[index].start(startValue, endValue, duration, callback, easingFunction);
}

template <typename T> void Animator<T>::stopAnimation(uint16_t index) {
    animations[index].stop();
}

template <typename T> void Animator<T>::pauseAnimation(uint16_t index) {
    AnimationContext& context = animations[index];
    if (context.state == AnimationState::Running) {
        context.state = AnimationState::Paused;
    }
}

template <typename T> void Animator<T>::resumeAnimation(uint16_t index) {
    AnimationContext& context = animations[index];
    if (context.state == AnimationState::Paused) {
        context.state = AnimationState::Running;
    }
}

template <typename T> void Animator<T>::loop()  {
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
            
            if (!context.isActive()) continue;

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
                context.remainingDuration = 0;
                context.state = AnimationState::Completed;
                param.state = context.state;
            }
            if (context.callback) {
                context.callback(param);
            }
        }
    }
}

// Explicitly declare the supported types
template class Animator<uint8_t>;
template class Animator<uint16_t>;
template class Animator<uint32_t>;
template class Animator<float>;