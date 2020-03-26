#pragma once
#include <Arduino.h>
#include "Easing.h"

namespace LightWeaver {

    enum class AnimationState {
        Started,
        Running,
        Paused,
        Completed,
        Stopped
    };

    struct AnimationParam {
        uint16_t index;
        float progress;
        float easedProgress;
        AnimationState state;
    };

    enum AnimatorTimescale {
        MILLISECOND = 1,
        CENTISECOND = 10,
        DECISECOND = 100,
        SECOND = 1000,
        DECASECOND = 10000
    };

    class Animator {
        typedef std::function<void(const AnimationParam&)> AnimationCallback;

        struct AnimationContext {
            uint16_t duration;
            uint16_t remainingDuration;
            bool looping;
            AnimationCallback callback;
            AnimationState state;
            EasingFunction easingFunction;

            AnimationContext(): duration(0), remainingDuration(0), looping(false), callback(nullptr), state(AnimationState::Stopped), easingFunction(nullptr) {};
            void start(uint16_t duration, bool looping, AnimationCallback callback, EasingFunction easingFunction);
            void stop();
            float progress();
            bool isActive();
            bool isRunning();
        };        

        uint16_t animationCount;
        uint16_t timescale;
        unsigned long previousTick;
        AnimationContext* animations;
        uint16_t currentlyRunning;
        
        public:
        Animator(uint16_t animationCount, uint16_t timescale = 1): animationCount(animationCount), timescale(timescale), previousTick(millis()), animations(new AnimationContext[animationCount]), currentlyRunning(0) {};
        ~Animator() { delete[] animations; }

        bool isAnimating();

        void pause();
        void resume();
        void stop();

        uint16_t startAnimation(uint16_t index, uint16_t duration, bool looping, AnimationCallback callback, EasingFunction easingFunction = Easing::Linear);
        uint16_t startAnimation(uint16_t duration, bool looping, AnimationCallback callback, EasingFunction easingFunction = Easing::Linear);
        void stopAnimation(uint16_t index);
        void pauseAnimation(uint16_t index);
        void resumeAnimation(uint16_t index);

        void loop();
    };
}