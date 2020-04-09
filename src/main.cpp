#include <Arduino.h>
#include <LightWeaver.h>
#include <LightWeaverWifi.h>
#include <LightWeaverHttpServer.h>
#include <LightWeaverMdns.h>
#include <LightWeaver/drivers/NeoRgbDriver.h>

using namespace LightWeaver;

#define PIXEL_COUNT 30
#define PIXEL_GROUP_SIZE 1
#define MAX_PLUGINS 4
#define DRIVER NeoRgbDriver<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod>

// LightWeaverCoreImpl<NeoDriverSK6812_RGBW<PIXEL_GROUP_SIZE>,MAX_PLUGINS> lightWeaver(PIXEL_COUNT, 255);
LightWeaverCoreImpl<DRIVER,MAX_PLUGINS> lightWeaver(PIXEL_COUNT, 255);
// LightWeaverCoreImpl<NoopDriver,MAX_PLUGINS> lightWeaver(PIXEL_COUNT, 255);

void setup()
{
    Serial.begin(115200);
    while(!Serial);

    lightWeaver.addPlugin<LightWeaverWifi>();
    lightWeaver.addPlugin<LightWeaverHttpServer>();
    lightWeaver.addPlugin<LightWeaverMdns>();
    lightWeaver.setup();
    lightWeaver.setBrightness(32);
}

void loop()
{
    lightWeaver.loop();
}