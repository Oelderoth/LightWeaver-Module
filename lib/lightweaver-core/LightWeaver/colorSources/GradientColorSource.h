#pragma once

#include <LightWeaver/ColorSource.h>
#include <LightWeaver/Easing.h>
#include <LightWeaver/Gradient.h>
#include <LightWeaver/PixelOffsetConfig.h>

namespace LightWeaver {

    class GradientColorSource : public ColorSource {
       private:
            uint16_t duration;
            bool loop;
            const Gradient colors;
            EasingFunction easing;

            const PixelOffsetConfig offsets;
            
            float progress;
            const Animation animation;

            void onAnimationTick(const AnimationParam& param) {
                progress = param.easedProgress;
            }
        public:

            GradientColorSource(uint32_t uid, const Gradient& colors, uint16_t duration, bool loop, EasingFunction easing = Easing::Linear, PixelOffsetConfig offsets = PixelOffsetConfig::withNone()) : 
                ColorSource(uid),
                duration(duration),
                loop(loop),
                colors(colors),
                easing(easing),
                offsets(offsets),
                progress(0.0f),
                animation(Animation(duration, loop, std::bind(&GradientColorSource::onAnimationTick, this, std::placeholders::_1), Easing::Linear)) {}

            virtual RgbaColor getColor() const {
                return colors.getColor(progress);
            }

            virtual RgbaColor getColor(uint8_t index, uint8_t count) const {
                float offset = offsets.getOffset(index, count);
                float offsetProgress = fmod(progress + offset, 1.0) + (offset < 0 ? 1 : 0);
                return colors.getColor(offsetProgress);
            }

            virtual ColorSource* clone() const {
                return new GradientColorSource(uid, colors, duration, loop, easing, offsets);
            }

            virtual const Animation* getAnimation() const {
                return &animation;
            }
    };
}