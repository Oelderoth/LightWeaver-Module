#include <Arduino.h>
#include <LightWeaver.h>
#include <LightWeaver/drivers/NeoDriverSK6812.h>

#include "SolidColorSource.h"
#include "FadeColorSource.h"
#include "OverlayColorSource.h"

using namespace LightWeaver;

#define PIXEL_COUNT 6
#define PIXEL_GROUP_SIZE 1

LightWeaverCore<NeoDriverSK6812_RGBW<PIXEL_GROUP_SIZE>> lightWeaver(PIXEL_COUNT, 255);

SolidRedColorSource solidColorSource = SolidRedColorSource(0x0004,
    LightWeaver::RgbColor(255,0,0));

SolidRedColorSource solidWhiteColorSource = SolidRedColorSource(0x0003,
    LightWeaver::RgbColor(255,255,255));

FadeColorSource fadeColorSource = FadeColorSource(0x0002,
    LightWeaver::RgbColor(255,0,0),
    LightWeaver::RgbColor(0,0,255),
    7000, true, Easing::Mirror(Easing::Linear));

FadeColorSource transparentFadeColorSource = FadeColorSource(0x0001,
    LightWeaver::RgbaColor(255,255,255,0),
    LightWeaver::RgbaColor(255,255,255,196),
    3333, true, Easing::Mirror(Easing::CubicInOut));

OverlayColorSource overlayColorSource = OverlayColorSource(0xFFFF, fadeColorSource, transparentFadeColorSource);

void setup()
{
    Serial.begin(115200);
    while(!Serial);
    
    lightWeaver.setup();
    lightWeaver.setColorSource(overlayColorSource);
    lightWeaver.setBrightness(255);
}

void loop()
{
    lightWeaver.loop();
}