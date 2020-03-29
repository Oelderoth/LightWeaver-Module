#include <Arduino.h>
#include <LightWeaver.h>
#include <LightWeaver/NeoDriverSK6812.h>
#include <LightWeaver/animation/Animator.h>

#include "SolidColorSource.h"
#include "FadeColorSource.h"

using namespace LightWeaver;

#define PIXEL_COUNT 6
#define PIXEL_GROUP_SIZE 1

LightWeaverCore<NeoDriverSK6812_RGBW<PIXEL_GROUP_SIZE>> lightWeaver(PIXEL_COUNT, 255);
Animator animator(2);

SolidRedColorSource solidColorSource = SolidRedColorSource(0x0002,
    LightWeaver::RgbColor(255,0,0));

SolidRedColorSource solidWhiteColorSource = SolidRedColorSource(0x0002,
    LightWeaver::RgbColor(255,255,255));

FadeColorSource fadeColorSource = FadeColorSource(0x0001,
    LightWeaver::RgbColor(0,0,0),
    LightWeaver::RgbColor(255,255,255),
    3000, true, Easing::Mirror(Easing::CubicInOut));

void setup()
{
    Serial.begin(115200);
    while(!Serial);
    
    lightWeaver.setup();
    lightWeaver.setColorSource(solidWhiteColorSource);
    lightWeaver.setBrightness(255);

    animator.setup();
    animator.playAnimation(Animation(5000,true,[](const AnimationParam& param) {
        if (param.progress == 1.0f) {
            if (param.iterations % 2 == 0) {
                // lightWeaver.setColorSource(fadeColorSource);
                lightWeaver.setBrightness(32);
            } else {
                // lightWeaver.setColorSource(solidColorSource);
                lightWeaver.setBrightness(255);
            }
        }
    }));
}

void loop()
{
    lightWeaver.loop();
    animator.loop();
}