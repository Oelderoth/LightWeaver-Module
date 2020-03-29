#include <Arduino.h>
#include <LightWeaver.h>
#include <LightWeaver/animation/Animator.h>

using namespace LightWeaver;

#define PIXEL_COUNT 6
#define PIXEL_GROUP_SIZE 1

LightWeaverCore<NeoDriverSK6812_RGBW<PIXEL_GROUP_SIZE>> lightWeaver(PIXEL_COUNT, 255);

void setup()
{
    Serial.begin(115200);
    while(!Serial);

    lightWeaver.setup();
    lightWeaver.setColorSource(GradientColorSource(0x00000003, 5000, 7, colors, Easing::Linear));
    lightWeaver.setBrightness(255);
}

void loop()
{
    lightWeaver.loop();
}