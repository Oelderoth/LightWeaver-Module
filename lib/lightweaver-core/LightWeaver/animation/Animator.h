#pragma once

#include "Animation.h"

namespace LightWeaver {
    struct Animator {
        public:
            struct AnimationHandle {
                private:
                    uint16_t animationIndex;
                    
                    AnimationHandle(uint16_t uid, uint16_t animationIndex): animationIndex(animationIndex), uid(uid) {}
                    friend class Animator;
                public:
                    uint16_t uid;
            };

            enum AnimatorTimescale {
                MILLISECOND = 1,
                CENTISECOND = 10,
                DECISECOND = 100,
                SECOND = 1000,
                DECASECOND = 10000
            };
        private:
            struct AnimationContext {
                // A random identifier generated at creation time
                // Used to ensure that an AnimationHandle is pointed at the correct AnimationContext
                uint16_t uid;
                Animation animation;
                AnimationState state;
                uint16_t remainingDuration;
                uint32_t lastTicked;

                void start(uint16_t uid, Animation animation, AnimationState state) {
                    this->uid = uid;
                    this->animation = animation;
                    this->state = state;
                    this->remainingDuration = animation.duration;
                }

                void stop() {
                    this->state = AnimationState::Stopped;
                }

                void pause() {
                    if (this->state == AnimationState::Running) {
                        this->state = AnimationState::Paused;
                    }
                }

                void resume() {
                    if (this->state == AnimationState::Paused) {
                        this->state = AnimationState::Running;
                    }
                }

                bool isActive() {
                    return state == AnimationState::Started || state == AnimationState::Running || state == AnimationState::Paused;
                }

                bool isRunning() {
                    return state == AnimationState::Started || state == AnimationState::Running;
                }

                void callback(const AnimationParam& param) {
                    if (animation.callback) {
                        animation.callback(param);
                    }
                }
            };

            uint16_t maxAnimations;
            AnimationContext* animations;
            AnimatorTimescale timescale;
            unsigned long previousTick;
            uint16_t currentlyRunningAnimations;

            AnimationHandle* playAnimation(uint16_t index, Animation& animation) const;
        public:
            Animator(uint16_t maxAnimations, AnimatorTimescale timescale = AnimatorTimescale::MILLISECOND): 
                maxAnimations(maxAnimations), 
                animations(new AnimationContext[maxAnimations]),
                timescale(timescale),
                previousTick(millis()),
                currentlyRunningAnimations(0) {}

            void setup();
            void loop();

            bool isAnimating();

            void pause();
            void resume();
            void stop();

            AnimationHandle* playAnimation(Animation& animation) const;
            void stopAnimation(AnimationHandle& animation) const;
            void pauseAnimation(AnimationHandle& animation) const;
            void resumeAnimation(AnimationHandle& animation) const;
    };
}