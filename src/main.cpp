#include <Arduino.h>
#include <LightWeaver.h>
#include <LightWeaverWifi.h>
#include <LightWeaverHttpServer.h>
#include <LightWeaverMdns.h>
#include <LightWeaver/drivers/NeoDriverSK6812.h>

using namespace LightWeaver;

#define PIXEL_COUNT 6
#define PIXEL_GROUP_SIZE 1
#define MAX_PLUGINS 4

LightWeaverCoreImpl<NeoDriverSK6812_RGBW<PIXEL_GROUP_SIZE>,MAX_PLUGINS> lightWeaver(PIXEL_COUNT, 255);
// LightWeaverCoreImpl<NoopDriver,MAX_PLUGINS> lightWeaver(PIXEL_COUNT, 255);

void setup()
{
    Serial.begin(115200);
    while(!Serial);

    lightWeaver.addPlugin<LightWeaverWifi>();
    lightWeaver.addPlugin<LightWeaverHttpServer>();
    lightWeaver.addPlugin<LightWeaverMdns>();
    lightWeaver.setup();
    lightWeaver.setBrightness(255);
}

void loop()
{
    lightWeaver.loop();
}