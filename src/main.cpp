#include <Arduino.h>

#include <LightWeaver.h>
#include <LightWeaver/NeoDriverSK6812.h>

using namespace LightWeaver;

#define PIXEL_COUNT 6
#define PIXEL_GROUP_SIZE 1

LightWeaverCore<NeoDriverSK6812_RGBW<PIXEL_GROUP_SIZE>> lightWeaver(PIXEL_COUNT);

void setup()
{
    lightWeaver.setup();
}

void loop()
{
    lightWeaver.loop();
}