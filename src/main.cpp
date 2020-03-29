#include <Arduino.h>
#include <LightWeaver.h>
#include <LightWeaver/NeoDriverSK6812.h>
#include <LightWeaver/animation/Animator.h>

#include "SolidColorSource.h"

using namespace LightWeaver;

#define PIXEL_COUNT 6
#define PIXEL_GROUP_SIZE 1

LightWeaverCore<NeoDriverSK6812_RGBW<PIXEL_GROUP_SIZE>> lightWeaver(PIXEL_COUNT, 255);

void setup()
{
    Serial.begin(115200);
    while(!Serial);

    lightWeaver.setup();
    lightWeaver.setColorSource(SolidRedColorSource(0x0001, LightWeaver::RgbColor(255,0,0)));
    lightWeaver.setBrightness(255);
}

void loop()
{
    lightWeaver.loop();
}