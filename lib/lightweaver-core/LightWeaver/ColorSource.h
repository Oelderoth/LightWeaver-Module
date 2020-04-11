#pragma once
#include <Arduino.h>

#include "Color.h"
#include "animation/Animation.h"

namespace LightWeaver {
    class ColorSource {
        public: 
            // A unique identifier for the color source
            // Used internally to compare and identify color sources
            uint32_t uid;

            ColorSource(uint32_t uid): uid(uid) {}

            virtual ~ColorSource() {}
            
            virtual RgbaColor getColor() const = 0;

            virtual RgbaColor getColor(uint8_t index, uint8_t count) const {
                return getColor();
            }

            virtual ColorSource* clone() const = 0;

            virtual const Animation* getAnimation() const {
                return nullptr;
            }
    };
}