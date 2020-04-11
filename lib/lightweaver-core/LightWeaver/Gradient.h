#pragma once

#include <LightWeaver/ColorSource.h>
#include <LightWeaver/Easing.h>
#include <LightWeaver/ColorSet.h>

namespace LightWeaver {
    struct Gradient {
        private:
            uint8_t getColorIndexBeforePosition(uint8_t position) const {
                if (position < colorPositions[0]) {
                    return 0;
                }
                for (uint8_t i = colorSet.size-1; i >= 0; i--) {
                    if (colorPositions[i] <= position) {
                        return i;
                    }
                }
                return colorSet.size-1;
            }

            float getPartialProgress(float progress, uint8_t indexBefore, uint8_t indexAfter) const {
                float beforePosition = colorPositions[indexBefore] / 255.0;
                float afterPosition = colorPositions[indexAfter] / 255.0;

                float diff = afterPosition - beforePosition;
                float partialProg = (progress - beforePosition) / diff;
                // Clamp between 0 and 1 to accomodate for errors introduced by floating point conversions
                return partialProg < 0 ? 0 : partialProg > 1 ? 1 : partialProg;
            }
        public:
            const ColorSet colorSet;
            std::unique_ptr<uint8_t[]> colorPositions;
            EasingFunction easing;
            Gradient(const ColorSet& colorSet, const uint8_t* colorPositions, EasingFunction easing = Easing::Linear): 
                colorSet(colorSet), 
                colorPositions(std::unique_ptr<uint8_t[]>(new uint8_t[colorSet.size])),
                easing(easing) {
                    if (colorPositions) {
                        for (uint8_t i = 0; i < colorSet.size; i++) {
                            this->colorPositions[i] = colorPositions[i];
                        }
                    } else {
                        if (colorSet.size == 1) {
                            this->colorPositions[0] = 0;
                        } else {
                            for (uint8_t i = 0; i < colorSet.size; i++) {
                                this->colorPositions[i] = 255 * i / (colorSet.size-1);
                            }
                        }
                    }
                };
            Gradient(const ColorSet& colorSet, EasingFunction easing = Easing::Linear) : 
                Gradient(colorSet, nullptr, easing) {};
            Gradient(const Gradient& other): Gradient(other.colorSet, other.colorPositions.get(), other.easing) {}

            RgbaColor getColor(const float progress) const {
                if (colorSet.size == 0) return RgbColor();
                if (colorSet.size == 1) return colorSet.colors[0];

                float easedProgress = easing(progress);

                uint8_t effectivePosition = (uint8_t)(easedProgress * 255);
                // Assume that any space before the first color is part of the first color
                // and any space after the last color is part of the last color
                // TODO: Allow optionally blending between last and first color
                if (effectivePosition <= colorPositions[0]) {
                    return colorSet.colors[0]; 
                } else if (effectivePosition >= colorPositions[colorSet.size - 1]) {
                    return colorSet.colors[colorSet.size - 1];
                }

                uint8_t beforeIndex = getColorIndexBeforePosition(effectivePosition);
                uint8_t afterIndex = beforeIndex == colorSet.size - 1 ? beforeIndex : beforeIndex + 1;
                float partialProgress = easing(getPartialProgress(easedProgress, beforeIndex, afterIndex));
                RgbaColor color = RgbaColor::linearBlend(colorSet.colors[beforeIndex], colorSet.colors[afterIndex], partialProgress);
                
                return color;
            }
    };
}