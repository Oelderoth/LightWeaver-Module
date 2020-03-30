#pragma once

#include "Animation.h"

namespace LightWeaver {
    struct Animator {
        public:
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
                AnimationState state = AnimationState::Stopped;
                uint16_t remainingDuration;
                uint32_t lastTicked;
                uint8_t iterations = 0;

                void start(uint16_t uid, Animation animation) {
                    this->uid = uid;
                    this->animation = animation;
                    this->state = AnimationState::Started;
                    this->remainingDuration = animation.duration;
                    this->iterations = 0;
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

                void loop() {
                    this->remainingDuration = this->animation.duration;
                    this->iterations++;
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
        public:
            Animator(uint16_t maxAnimations, AnimatorTimescale timescale = AnimatorTimescale::MILLISECOND): 
                maxAnimations(maxAnimations >= 0xFFFF ? 0xFFFF-1 : maxAnimations), 
                animations(new AnimationContext[maxAnimations >= 0xFFFF ? 0xFFFF-1 : maxAnimations]),
                timescale(timescale),
                previousTick(millis()),
                currentlyRunningAnimations(0) {}

            void setup();
            void loop();

            bool isAnimating();

            void pause();
            void resume();
            void stop();
            
            uint16_t playAnimation(const uint16_t index, const Animation& animation) const;
            uint16_t playAnimation(const uint16_t index, const Animation* animation) const;
            uint16_t playAnimation(const Animation& animation) const;
            uint16_t playAnimation(const Animation* animation) const;
            void stopAnimation(uint16_t animation) const;
            void pauseAnimation(uint16_t animation) const;
            void resumeAnimation(uint16_t animation) const;
    };
}