#pragma once
#include <Arduino.h>
#include "Easing.h"

namespace LightWeaver {

    enum AnimationState {
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

    template<typename T>
    class Animator {
        typedef std::function<void(const AnimationParam&)> AnimationCallback;

        struct AnimationContext {
            T startValue;
            T endValue;
            uint16_t duration;
            uint16_t remainingDuration;
            AnimationCallback callback;
            AnimationState state;
            EasingFunction easingFunction;

            AnimationContext(): startValue(T()), endValue(T()), duration(0), remainingDuration(0), callback(nullptr), state(AnimationState::Stopped), easingFunction(nullptr) {};
            void start(T startValue, T endValue, uint16_t duration, AnimationCallback callback, EasingFunction easingFunction);
            void stop();
            float progress();
            bool isActive();
        };        

        uint16_t animationCount;
        uint16_t timescale;
        unsigned long previousTick;
        AnimationContext* animations;
        uint16_t currentlyRunning;

        T interpolateValue(T start, T end, float progress) {
            return (end - start) * progress + start;
        }
        
        public:
        Animator(uint16_t animationCount, uint16_t timescale = 1): animationCount(animationCount), timescale(timescale), previousTick(millis()), animations(new AnimationContext[animationCount]), currentlyRunning(0) {};
        ~Animator() { delete[] animations; }

        bool isAnimating();

        void pause();
        void resume();
        void stop();

        void startAnimation(uint16_t index, T startValue, T endValue, uint16_t duration, AnimationCallback callback, EasingFunction easingFunction = Easing::Linear);
        void stopAnimation(uint16_t index);
        void pauseAnimation(uint16_t index);
        void resumeAnimation(uint16_t index);

        void loop();
    };
}