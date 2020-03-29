#include <Arduino.h>
#include <LightWeaver.h>
#include <LightWeaver/NeoDriverSK6812.h>
#include <LightWeaver/animation/Animator.h>

#include "FadeColorSource.h"

using namespace LightWeaver;

#define PIXEL_COUNT 6
#define PIXEL_GROUP_SIZE 1

LightWeaverCore<NeoDriverSK6812_RGBW<PIXEL_GROUP_SIZE>> lightWeaver(PIXEL_COUNT, 255);

void setup()
{
    Serial.begin(115200);
    while(!Serial);

    FadeColorSource colorSource = FadeColorSource(0x0001, 
        LightWeaver::RgbColor(0,0,0),
        LightWeaver::RgbColor(255,255,255),
        5000, true, Easing::Mirror(Easing::Linear));

    lightWeaver.setup();
    lightWeaver.setColorSource(colorSource);
    lightWeaver.setBrightness(255);
}

void loop()
{
    lightWeaver.loop();
}