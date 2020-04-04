#pragma once

#include <memory>
#include <Arduino.h>
#include "Color.h"

namespace LightWeaver {
    struct ColorSet {
        uint8_t size;
        std::unique_ptr<RgbaColor[]> colors;

        ColorSet(uint8_t size, const RgbaColor* colors) :
            size(size),
            colors(std::unique_ptr<RgbaColor[]>(new RgbaColor[size])) {
                if (colors) {
                    for (uint8_t i = 0; i < size; i++) {
                        this->colors[i] = colors[i];
                    }
                }
            }

        ColorSet(): size(0), colors(nullptr) {}
        ColorSet(const ColorSet& other): ColorSet(other.size, other.colors.get()) {}
    };
}