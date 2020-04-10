#pragma once

#include <LightWeaver/ColorSource.h>

namespace LightWeaver {
    class SolidColorSource : public ColorSource {
        private:
            HslaColor color;
        public:
            SolidColorSource(uint32_t uid, RgbaColor color) : 
                ColorSource(uid),
                color(color) {}

            virtual RgbaColor getColor() const {
                return color;
            }

            virtual ColorSource* clone() const {
                return new SolidColorSource(uid, color);
            }
    };
}