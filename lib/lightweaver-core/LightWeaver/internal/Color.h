#pragma once
#include <Arduino.h>

namespace LightWeaver {
    struct RgbColor {
        RgbColor(uint8_t r, uint8_t g, uint8_t b) : R(r), G(g), B(b) {};
        uint8_t R;
        uint8_t G;
        uint8_t B;
    };

    struct HsbColor {
        HsbColor(uint8_t h, uint8_t s, uint8_t b) : H(h), S(s), B(b) {};
        float H;
        float S;
        float B;
    };
};