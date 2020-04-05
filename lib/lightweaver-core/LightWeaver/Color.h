#pragma once
#include <Arduino.h>

namespace LightWeaver {
    struct RgbColor;
    struct RgbaColor;
    struct HsvaColor;
    struct HslaColor;

    struct RgbColor {
        RgbColor(uint8_t r, uint8_t g, uint8_t b) : R(r), G(g), B(b) {};
        RgbColor(): RgbColor(0,0,0) {};
        RgbColor(const RgbaColor& rgba);
        RgbColor(const HsvaColor& hsv);
        RgbColor(const HslaColor& hsv);

        uint8_t R;
        uint8_t G;
        uint8_t B;

        static RgbColor linearBlend(const RgbColor& start, const RgbColor& end, float progress);
    };

    struct RgbaColor {
        RgbaColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : R(r), G(g), B(b), A(a) {};
        RgbaColor(): RgbaColor(0,0,0,0) {};
        RgbaColor(const RgbColor& rgb);
        RgbaColor(const HsvaColor& rgb);
        RgbaColor(const HslaColor& rgb);

        uint8_t R;
        uint8_t G;
        uint8_t B;
        uint8_t A;

        static RgbaColor linearBlend(const RgbaColor& start, const RgbaColor& end, float progress);
    };

    struct HsvaColor {
        HsvaColor(float h, float s, float v, uint8_t a = 255): H(h), S(s), V(v), A(a) {};
        HsvaColor(): HsvaColor(0,0,0) {};
        HsvaColor(const RgbColor& rgb);
        HsvaColor(const RgbaColor& rgb);
        HsvaColor(const HslaColor& rgb);

        float H;
        float S;
        float V;
        uint8_t A;
    };

    struct HslaColor {
        HslaColor(float h, float s, float l, uint8_t a = 255): H(h), S(s), L(l), A(a) {};
        HslaColor(): HslaColor(0,0,0) {};
        HslaColor(const RgbColor& rgb);
        HslaColor(const RgbaColor& rgb);
        HslaColor(const HsvaColor& rgb);

        float H;
        float S;
        float L;
        uint8_t A;
    };
}