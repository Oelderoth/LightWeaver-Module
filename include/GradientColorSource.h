#pragma once

#include <Arduino.h>
#include "LightWeaver.h"
#include "LightWeaver/internal/ColorSource.h"
using namespace LightWeaver;

class GradientColorSource : public ColorSource {

    static float LinearEase(float val) {
        return val;
    }

    uint8_t colorCount;
    LightWeaver::RgbColor* colors;
    uint8_t* colorPositions;
    float (*easingFunction)(float);

    uint8_t getColorIndexBefore(uint8_t effectivePosition) const {
        if (effectivePosition < colorPositions[0]) {
            return 0;
        }
        for (uint8_t i = colorCount-1; i >= 0; i--) {
            if (colorPositions[i] <= effectivePosition) {
                return i;
            }
        }
        return colorCount-1;
    }

    float getPartialProgress(float progress, uint8_t indexBefore, uint8_t indexAfter) const {
        // If we have positions defined, use those
        // Otherwise, assume all colors are evenly distributed, with the first and last colors at 0 and 255 respectively
        float beforePosition = colorPositions[indexBefore] / 255.0;
        float afterPosition = colorPositions[indexAfter] / 255.0;

        float diff = afterPosition - beforePosition;
        float partialProg = (progress - beforePosition) / diff;
        // Clamp between 0 and 1 to accomodate for errors introduced by floating point conversions
        return partialProg < 0 ? 0 : partialProg > 1 ? 1 : partialProg;
    }

    public:

    enum class GradientType {
        LINEAR = 0,
        QUADRATIC = 1,
        CUBIC = 2
    };

    GradientColorSource(uint32_t identifier, uint16_t duration, uint8_t colorCount, const LightWeaver::RgbColor _colors[], const uint8_t _colorPositions[], float (*easingFunction)(float)) 
        : ColorSource(identifier, duration), colorCount(colorCount), easingFunction(easingFunction) {
        colors = new LightWeaver::RgbColor[colorCount];
        memcpy(colors, _colors, sizeof(LightWeaver::RgbColor) * colorCount);

        colorPositions = new uint8_t[colorCount];
        if (_colorPositions == nullptr) {
            // If positions aren't specified, evenly distribute colors with the first and last colors at 0 and 255 respectively
            for (uint8_t i = 0; i < colorCount; i++) {
                colorPositions[i] = 255 * i / (colorCount-1);
            }
        } else {    
            memcpy(colorPositions, _colorPositions, sizeof(uint8_t) * colorCount);
        }
    }
    
    GradientColorSource(uint32_t identifier, uint16_t duration, uint8_t colorCount, const LightWeaver::RgbColor _colors[], const uint8_t _colorPositions[]) : GradientColorSource(identifier, duration, colorCount, _colors, _colorPositions, GradientColorSource::LinearEase) {};
    GradientColorSource(uint32_t identifier, uint16_t duration, uint8_t colorCount, const LightWeaver::RgbColor _colors[], float (*easingFunction)(float)) : GradientColorSource(identifier, duration, colorCount, _colors, nullptr, easingFunction) {};
    GradientColorSource(uint32_t identifier, uint16_t duration, uint8_t colorCount, const LightWeaver::RgbColor _colors[]) : GradientColorSource(identifier, duration, colorCount, _colors, nullptr, GradientColorSource::LinearEase) {};

    ~GradientColorSource() {
        delete[] colors;
        delete[] colorPositions;

        colors = nullptr;
        colorPositions = nullptr;
    }

    LightWeaver::RgbaColor getColor(float progress) const {
        uint8_t effectivePosition = (int)(progress * 255);

        // Assume that any space before the first color is part of the first color
        // and any space after the last color is part of the last color
        // TODO: Allow optionally blending between last and first color
        if (effectivePosition <= colorPositions[0]) {
           return colors[0]; 
        } else if (effectivePosition >= colorPositions[colorCount-1]) {
            return colors[colorCount-1];
        }

        uint8_t beforeIndex = getColorIndexBefore(effectivePosition);
        uint8_t afterIndex = beforeIndex == colorCount - 1 ? beforeIndex : beforeIndex + 1;

        float partialProgress = easingFunction(getPartialProgress(progress, beforeIndex, afterIndex));
        return LightWeaver::RgbColor::linearBlend(colors[beforeIndex], colors[afterIndex], partialProgress);
    }
    
    GradientColorSource* clone() const {
        return new GradientColorSource(identifier, duration, colorCount, colors, colorPositions, easingFunction);
    }
};