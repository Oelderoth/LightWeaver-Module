#pragma once
#include <Arduino.h>
#include "internal/Color.h"
#include "animation/Animation.h"

namespace LightWeaver {
    class ColorSource {
        // A unique identifier for the color source
        // Used internally to compare and identify color sources
        uint32_t uid;

        virtual ~ColorSource() {}
        
        virtual RgbaColor getColor() const = 0;

        virtual ColorSource* clone() const = 0;

        virtual Animation const * const getAnimation() const {
            return nullptr;
        }
    };
}