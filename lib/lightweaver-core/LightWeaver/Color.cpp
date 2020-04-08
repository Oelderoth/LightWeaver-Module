#include "Color.h"

namespace LightWeaver {
    /**
     * The easiest way to handle the wide variety of conversions between color types
     * for the majority of cases is to define a conversion so that each color type can
     * be implicitly converted to/from RgbaColor, which is the de facto "default" color type
     * (since LEDs function using RGB values, and RgbaColor preserves alpha information while RgbColor does not).
     * These conversions can then be explicitly called to enable implicit casts between other types
     * 
     * In cases where this approach would be unnecessarily costly or the conversion directly between
     * types is trivial, then a direct conversion should be preferred;
     */
    // RGB <=> RGBA
    RgbaColor::RgbaColor(const RgbColor& rgb): RgbaColor(rgb.R, rgb.G, rgb.B, 255) {};
    RgbColor::RgbColor(const RgbaColor& rgba): RgbColor(rgba.R, rgba.G, rgba.B) {};
    RgbColor::RgbColor(const HsvaColor& hsva): RgbColor(RgbaColor(hsva)) {};
    RgbColor::RgbColor(const HslaColor& hsla): RgbColor(RgbaColor(hsla)) {};
    // HSVA <=> RGBA
    RgbaColor::RgbaColor(const HsvaColor& hsva) {
        float h = fmod(hsva.H,360.f);
        float c = hsva.V * hsva.S;
        float x = c * (1 - fabs(fmod(h/60,2)-1));
        float m = hsva.V - c;
        if (h >= 0 && h < 60) {
            R = (c + m) * 255;
            G = (x + m) * 255;
            B = 0;
        } else if (h >= 60 && h < 120) {
            R = (x + m) * 255;
            G = (c + m) * 255;
            B = 0;
        } else if (h >= 120 && h < 180) {
            R = 0;
            G = (c + m) * 255;
            B = (x + m) * 255;
        } else if (h >= 180 && h < 240) {
            R = 0;
            G = (x + m) * 255;
            B = (c + m) * 255;
        } else if (h >= 240 && h < 300) {
            R = (x + m) * 255;
            G = 0;
            B = (c + m) * 255;
        } else {
            R = (c + m) * 255;
            G = 0;
            B = (x + m) * 255;
        };
        A = hsva.A;
    };
    HsvaColor::HsvaColor(const RgbaColor& rgba) {
        float r = rgba.R / 255.0;
        float g = rgba.G / 255.0;
        float b = rgba.B / 255.0;
        float cMax = fmax(fmax(r,g),b);
        float cMin = fmin(fmin(r,g),b);
        float delta = cMax - cMin;
        if (delta == 0) {
            H = 0;
        } else if (cMax == r) {
            H = 60.0 * fmod((g - b)/delta,6);
        } else if (cMax == g) {
            H = 60.0 * ((b - r)/delta + 2);
        } else if (cMax == b) {
            H = 60.0 * ((r - g)/delta + 4);
        }

        if (cMax == 0) {
            S = 0;
        } else {
            S = delta/cMax;
        }

        V = cMax;
        A = rgba.A;
    };
    HsvaColor::HsvaColor(const RgbColor& rgb): HsvaColor(RgbaColor(rgb)) {};
    HsvaColor::HsvaColor(const HslaColor& hsla): HsvaColor(RgbaColor(hsla)) {};
    // HSLA <=> RGBA
    RgbaColor::RgbaColor(const HslaColor& hsla) {
        float h = fmod(hsla.H,360.f);
        float C = (1 - fabs(2 * hsla.L - 1)) * hsla.S;
        float H = h / 60;
        float X = C * (1 - fabs(fmod(H,2) - 1));
        float m = hsla.L - (C / 2);
        if (H >= 0 && H < 1) {
            R = 255 * (C + m);
            G = 255 * (X + m);
            B = 255 * (m);
        } else if (H >= 1 && H < 2) {
            R = 255 * (X + m);
            G = 255 * (C + m);
            B = 255 * (m);
        } else if (H >= 2 && H < 3) {
            R = 255 * (m);
            G = 255 * (C+m);
            B = 255 * (X+m);
        } else if (H >= 3 &&  H < 4) {
            R = 255 * (m);
            G = 255 * (X+m);
            B = 255 * (C+m);
        } else if (H >= 4 && H < 5) { 
            R = 255 * (X+m);
            G = 255 * (m);
            B = 255 * (C+m);
        } else {
            R = 255 * (C+m);
            G = 255 * (m);
            B = 255 * (X+m);
        }
    };
    HslaColor::HslaColor(const RgbaColor& rgba) {
        float r = rgba.R / 255.0;
        float g = rgba.G / 255.0;
        float b = rgba.B / 255.0;
        float cMax = fmax(fmax(r,g),b);
        float cMin = fmin(fmin(r,g),b);
        float delta = cMax - cMin;
        if (delta == 0) {
            H = 0;
        } else if (cMax == r) {
            H = 60.0 * fmod((g - b)/delta,6);
        } else if (cMax == g) {
            H = 60.0 * ((b - r)/delta + 2);
        } else if (cMax == b) {
            H = 60.0 * ((r - g)/delta + 4);
        }

        L = (cMax + cMin) / 2;

        if (cMax == 0) {
            S = 0;
        } else {
            S = delta/(1-fabs(2*L - 1));
        }

        A = rgba.A;
    };
    HslaColor::HslaColor(const RgbColor& rgb): HslaColor(RgbaColor(rgb)) {};
    HslaColor::HslaColor(const HsvaColor& hsva): HslaColor(RgbaColor(hsva)) {};

    RgbColor RgbColor::linearBlend(const RgbColor& start, const RgbColor& end, float progress)
    {
        return RgbColor((end.R - start.R) * progress + start.R, 
            (end.G - start.G) * progress + start.G,
            (end.B - start.B) * progress + start.B);
    }

    RgbaColor RgbaColor::linearBlend(const RgbaColor& start, const RgbaColor& end, float progress)
    {
        return RgbaColor((end.R - start.R) * progress + start.R, 
            (end.G - start.G) * progress + start.G,
            (end.B - start.B) * progress + start.B,
            (end.A - start.A) * progress + start.A);
    }
}