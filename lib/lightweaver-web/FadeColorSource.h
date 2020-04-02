#pragma once

#include <LightWeaver/ColorSource.h>
#include <LightWeaver/Easing.h>

namespace LightWeaverServer{
    using namespace LightWeaver;

    class FadeColorSource : public ColorSource {
        private:
            RgbaColor start;
            RgbaColor end;
            uint16_t duration;
            bool loop;
            EasingFunction easing;
            float progress;
            Animation animation;

            void onAnimationTick(const AnimationParam& param) {
                progress = param.easedProgress;
            }
            
            FadeColorSource(uint32_t uid, RgbaColor start, RgbaColor end, uint16_t duration, bool loop, EasingFunction easing, float progress) : 
                ColorSource(uid),
                start(start),
                end(end),
                duration(duration),
                loop(loop),
                easing(easing),
                progress(0.0f),
                animation(Animation(duration, loop, std::bind(&FadeColorSource::onAnimationTick, this, std::placeholders::_1), easing)) {}
        public:

            FadeColorSource(uint32_t uid, RgbaColor start, RgbaColor end, uint16_t duration, bool loop, EasingFunction easing = Easing::Linear) : 
                FadeColorSource(uid, start, end, duration, loop, easing, 0.0f) {}

            virtual RgbaColor getColor() const {
                return RgbaColor::linearBlend(start, end, progress);
            }

            virtual ColorSource* clone() const {
                return new FadeColorSource(uid, start, end, duration, loop, easing, progress);
            }

            virtual const Animation* getAnimation() const {
                return &animation;
            }
    };
}