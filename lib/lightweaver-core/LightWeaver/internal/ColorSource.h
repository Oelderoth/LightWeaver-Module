#pragma once
#include <Arduino.h>
#include "Color.h"
#include "Features.h"

namespace LightWeaver {
    struct ColorSource {
        // A unique identifier for the animation
        // Used to determine when an animation has changed vs when a new animation has been created
        uint32_t identifier;

        // Duration of the ColorSource's animation in ms
        // A duration of 0 means that the source is not animated, and will always receive a progress of 1.0 
        uint16_t duration;
        
        ColorSource(uint32_t identifier, uint16_t duration) : identifier(identifier), duration(duration) {};

        // Virtual destructor to allow implementations to cleanup
        virtual ~ColorSource() {};

        virtual RgbaColor getColor(float progress) const = 0;
        
        virtual ColorSource* clone() const = 0;
    };
}