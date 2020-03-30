#pragma once

#include <LightWeaver/ColorSource.h>

class SolidRedColorSource : public LightWeaver::ColorSource {
    private:
        LightWeaver::RgbaColor color;
    public:
        SolidRedColorSource(uint32_t uid, LightWeaver::RgbaColor color) : 
            LightWeaver::ColorSource(uid),
            color(color) {}

        virtual LightWeaver::RgbaColor getColor() const {
            return color;
        }

        virtual ColorSource* clone() const {
            return new SolidRedColorSource(uid, color);
        }
};