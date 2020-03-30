#pragma once

#include <Arduino.h>

#include "../Easing.h"

namespace LightWeaver {
    enum class AnimationState {
        Started,
        Running,
        Paused,
        Completed,
        Stopped
    };

    struct AnimationParam {
        float progress;
        float easedProgress;
        uint8_t iterations;
        AnimationState state;

        AnimationParam(float progress, float easedProgress, uint8_t iterations, AnimationState state): 
            progress(progress), 
            easedProgress(easedProgress), 
            iterations(iterations),
            state(state) {}
    };

    typedef std::function<void(const AnimationParam&)> AnimationCallback;

    struct Animation {
        // How many ticks of the specified timescale should it take for 
        // the progress of the animation to go from 0.0 to 1.0
        // A value of 0 will cause the progress to always be 1.0
        uint16_t duration;

        // Whether the animation should loop
        bool loop;

        // This callback function is called roughly every timescale
        // with the current progress of the animation
        AnimationCallback callback;

        EasingFunction easingFunction;

        Animation(): Animation(0, false, nullptr) {}
        Animation(uint16_t duration, bool loop, AnimationCallback callback, EasingFunction easingFunction = Easing::Linear):
            duration(duration),
            loop(loop),
            callback(callback),
            easingFunction(easingFunction) {}
    };
}