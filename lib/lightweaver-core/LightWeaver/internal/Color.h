#pragma once
#include <Arduino.h>

namespace LightWeaver {
    struct RgbColor;
    struct RgbaColor;

    struct RgbColor {
        RgbColor(uint8_t r, uint8_t g, uint8_t b) : R(r), G(g), B(b) {};
        RgbColor(): RgbColor(0,0,0) {};
        RgbColor(const RgbaColor& rgba);

        uint8_t R;
        uint8_t G;
        uint8_t B;

        static RgbColor linearBlend(const RgbColor& start, const RgbColor& end, float progress);
    };

    struct RgbaColor {
        RgbaColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : R(r), G(g), B(b), A(a) {};
        RgbaColor(): RgbaColor(0,0,0,0) {};
        RgbaColor(const RgbColor& rgb);

        uint8_t R;
        uint8_t G;
        uint8_t B;
        uint8_t A;

        static RgbaColor linearBlend(const RgbaColor& start, const RgbaColor& end, float progress);
    };
}