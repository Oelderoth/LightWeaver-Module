#include <Arduino.h>
#include <LightWeaver.h>
#include <LightWeaver/drivers/NeoDriverSK6812.h>

#include "SolidColorSource.h"
#include "FadeColorSource.h"
#include "OverlayColorSource.h"

using namespace LightWeaver;

#define PIXEL_COUNT 6
#define PIXEL_GROUP_SIZE 1

LightWeaverCoreImpl<NeoDriverSK6812_RGBW<PIXEL_GROUP_SIZE>,2> lightWeaver(PIXEL_COUNT, 255);

void setup()
{
    Serial.begin(115200);
    while(!Serial);
    
    lightWeaver.setup();
    lightWeaver.setBrightness(255);
}

void loop()
{
    lightWeaver.loop();
}