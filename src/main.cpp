#include <Arduino.h>

#include <LightWeaver.h>
#include <LightWeaver/NeoDriverSK6812.h>

using namespace LightWeaver;

#define PIXEL_COUNT 6
#define PIXEL_GROUP_SIZE 1

LightWeaverCore<NeoDriverSK6812_RGBW<PIXEL_GROUP_SIZE>> lightWeaver(PIXEL_COUNT);

class SolidRedColorSource : public ColorSource {
    public:
    SolidRedColorSource(uint32_t identifier) : ColorSource(identifier, 0) {}

    LightWeaver::RgbColor getColor(float progress) const {
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

    lightWeaver.setup();
    SolidRedColorSource red = SolidRedColorSource(0x0000);
    lightWeaver.setColorSource(red);
    lightWeaver.setBrightness(32);
}

void loop()
{
    lightWeaver.loop();
}