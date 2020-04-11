#pragma once

#include <LightWeaver/animation/Animator.h>
#include <LightWeaver/ColorSource.h>
#include <LightWeaver/Easing.h>
#include <LightWeaver/PixelOffsetConfig.h>

namespace LightWeaver {
    class HsvMeanderColorSource : public ColorSource {
        private:
            Animator animator{1,Animator::AnimatorTimescale::MILLISECOND};

            HsvaColor color;
            uint16_t duration;
            float hueDistance;
            float saturationDistance;
            float valueDistance;

            const PixelOffsetConfig pixelOffsets;

            float progress;
            Animation animation;

            void onAnimationTick(const AnimationParam& param) {
                progress = param.progress;
            }

            const float getOffset(float progress) const {
                float x = fmod(progress, 1.0) * 96.5;
                float offset = (sin(x) + sin(0.3*x) + sin(0.2 * x) + sin(0.15 * x) + sin(0.9*x))/5.5f;
                return offset;
            }

        public:
            HsvMeanderColorSource(uint32_t uid, HsvaColor color, uint16_t duration, float hueDistance, float saturationDistance, float valueDistance, const PixelOffsetConfig pixelOffsets) : 
                ColorSource(uid),
                color(color),
                duration(duration),
                hueDistance(hueDistance),
                saturationDistance(saturationDistance),
                valueDistance(valueDistance),
                pixelOffsets(pixelOffsets),
                progress(0.0f),
                animation(Animation(duration, true, std::bind(&HsvMeanderColorSource::onAnimationTick, this, std::placeholders::_1), Easing::Linear)) {
                    animator.setup();
                }
            
            virtual RgbaColor getColor() const {
                float h = color.H + getOffset(progress) * hueDistance;
                float s = color.S + getOffset(1-progress) * saturationDistance;
                float v = color.V + getOffset(fmod(0.5+progress, 1.0f)) * valueDistance;
                return HsvaColor(h, s < 0 ? 0 : s > 1 ? 1 : s, v < 0 ? 0 : v > 1 ? 1 : v);
            }

            virtual RgbaColor getColor(uint8_t index, uint8_t count) const {
                float offset = pixelOffsets.getOffset(index,count);
                float p = fmod(progress + offset, 1.0f) + (offset < 0 ? 1 : 0);
                float h = color.H + getOffset(p) * hueDistance;
                float offS = (getOffset(1-p) + 1)/2;
                float offV = (getOffset(0.5 + p) + 1)/2;
                float minS = color.S < saturationDistance ? 0 : color.S - saturationDistance;
                float maxS = color.S > 1 - saturationDistance ? 1 : color.S + saturationDistance;

                float minV = color.V < valueDistance ? 0 : color.V - valueDistance;
                float maxV = color.V > 1 - valueDistance ? 1 : color.V + valueDistance;

                float s = minS + (maxS - minS) * offS;
                float v = minV + (maxV - minV) * offV;
                return HsvaColor(h, s < 0 ? 0 : s > 1 ? 1 : s, v < 0 ? 0 : v > 1 ? 1 : v);
            }

            virtual ColorSource* clone() const {
                return new HsvMeanderColorSource(uid, color, duration, hueDistance, saturationDistance, valueDistance, pixelOffsets);
            }

            virtual const Animation* getAnimation() const {
                return &animation;
            }
    };
}