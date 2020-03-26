#include <Arduino.h>

#include <LightWeaver.h>
#include <LightWeaver/NeoDriverSK6812.h>

#include "GradientColorSource.h"

using namespace LightWeaver;

#define PIXEL_COUNT 6
#define PIXEL_GROUP_SIZE 1

LightWeaverCore<NeoDriverSK6812_RGBW<PIXEL_GROUP_SIZE>> lightWeaver(PIXEL_COUNT, 255);

class SolidRedColorSource : public ColorSource {
    public:
    SolidRedColorSource(uint32_t identifier) : ColorSource(identifier, 0) {}

    LightWeaver::RgbaColor getColor(float progress) const {
        return LightWeaver::RgbColor(255,0,0);
    }

    ColorSource* clone() const {
        return new SolidRedColorSource(identifier);
    }
};

void setup()
{
    Serial.begin(115200);
    while(!Serial);

    LightWeaver::RgbColor colors[] = {
        LightWeaver::RgbColor(255, 000, 000),
        LightWeaver::RgbColor(255, 255, 000),
        LightWeaver::RgbColor(000, 255, 000),
        LightWeaver::RgbColor(000, 255, 255),
        LightWeaver::RgbColor(000, 000, 255),
        LightWeaver::RgbColor(255, 000, 255),
        LightWeaver::RgbColor(255, 000, 000)
    };

    // LightWeaver::RgbColor whiteFlash[] = {
    //     LightWeaver::RgbColor(000, 000, 000),
    //     LightWeaver::RgbColor(255, 255, 255),
    //     LightWeaver::RgbColor(000, 000, 000),
    //     LightWeaver::RgbColor(000, 000, 000)
    // };

    // uint8_t whiteFlashPositions[] = {
    //     0,
    //     32,
    //     128,
    //     255
    // };

    lightWeaver.setup();
    lightWeaver.setColorSource(GradientColorSource(0x00000003, 5000, 7, colors, Easing::Linear));
    lightWeaver.setBrightness(255);
}

void loop()
{
    lightWeaver.loop();
}