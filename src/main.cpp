#include <Arduino.h>

#include <LightWeaver.h>
#include <LightWeaver/NeoDriverSK6812.h>

#define PIXEL_COUNT 6
#define PIXEL_GROUP_SIZE 1

LightWeaverCore<NeoDriverSK6812<PIXEL_GROUP_SIZE>> lightWeaver(PIXEL_COUNT);

void setup()
{
    lightWeaver.setup();
}

void loop()
{
    lightWeaver.loop();
}