#pragma once

#include <LightWeaver/animation/Animator.h>
#include <LightWeaver/ColorSource.h>
#include <LightWeaver/Easing.h>

namespace LightWeaver {
    class HueMeanderColorSource : public ColorSource {
        private:
            template<typename T>
            struct Transition {
                T start;
                T end;
                float progress;
                Transition(T start, T end): start(start), end(end), progress(0) {};
            };

            Animator animator{2,Animator::AnimatorTimescale::MILLISECOND};

            HsvaColor color;
            float maxDistance;
            uint16_t maxDuration;
            EasingFunction easing;

            float progress;
            Animation everyTickAnimation;

            Transition<float> hueTransition;

            void onLoop(const AnimationParam& param) {
                animator.loop();
            }

            void onAnimationTick(const AnimationParam& param) {
                hueTransition.progress = param.easedProgress;
                if (param.state == AnimationState::Completed) {
                    beginHueTransition();
                }
            }

            void beginHueTransition() {
                float startValue = hueTransition.end;
                float endValue = random(-maxDistance,maxDistance) + color.H;
                float diff = fabs(startValue - endValue) / (2*maxDistance);
                uint16_t duration = diff * maxDuration;
                hueTransition = Transition<float>(startValue,endValue);
                animator.playAnimation(Animation(duration,false,std::bind(&HueMeanderColorSource::onAnimationTick,this,std::placeholders::_1),easing));
            }
        public:
            HueMeanderColorSource(uint32_t uid, HsvaColor color, float maxDistance, uint16_t maxDuration, EasingFunction easing = Easing::Linear) : 
                ColorSource(uid),
                color(color),
                maxDistance(maxDistance),
                maxDuration(maxDuration),
                easing(easing),
                progress(0.0f),
                everyTickAnimation(Animation(0, true, std::bind(&HueMeanderColorSource::onLoop, this, std::placeholders::_1), Easing::Linear)),
                hueTransition(Transition<float>(color.H,color.H)) {
                    animator.setup();
                    beginHueTransition();
                }
            
            virtual RgbaColor getColor() const {
                float h = (hueTransition.end - hueTransition.start) * hueTransition.progress + hueTransition.start;
                return HsvaColor(h, color.S, color.V);
            }

            virtual ColorSource* clone() const {
                return new HueMeanderColorSource(uid, color, maxDistance, maxDuration, easing);
            }

            virtual const Animation* getAnimation() const {
                return &everyTickAnimation;
            }
    };
}