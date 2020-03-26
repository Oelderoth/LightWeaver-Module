#include "Color.h"

namespace LightWeaver {
    RgbColor::RgbColor(const RgbaColor& rgba): RgbColor(rgba.R, rgba.G, rgba.B) {};
    RgbaColor::RgbaColor(const RgbColor& rgb): RgbaColor(rgb.R, rgb.G, rgb.B, 255) {};

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