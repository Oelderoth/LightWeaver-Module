#include <Arduino.h>

#include <LightWeaver.h>
#include <LightWeaver/NeoDriverSK6812.h>

#include "GradientColorSource.h"

using namespace LightWeaver;

#define PIXEL_COUNT 6
#define PIXEL_GROUP_SIZE 1

LightWeaverCore<NeoDriverSK6812_RGBW<PIXEL_GROUP_SIZE>> lightWeaver(PIXEL_COUNT);
Animator<float> intAnimator(4, AnimatorTimescale::CENTISECOND);
int i = 1;

void handle(const AnimationParam& param) {
    Serial.println(param.easedProgress);
    if (param.state == AnimationState::Completed) {
        Serial.println("Done");
        switch (++i) {
            case 2:
                intAnimator.startAnimation(2,0,1.0f,100,handle, Easing::QuadraticIn);
                break;
            case 3:
                intAnimator.startAnimation(2,0,1.0f,100,handle, Easing::QuadraticOut);
                break;
            case 4:
                intAnimator.startAnimation(2,0,1.0f,100,handle, Easing::CubicIn);
                break;
            case 5:
                intAnimator.startAnimation(2,0,1.0f,100,handle, Easing::CubicOut);
                break;
            case 6:
                intAnimator.startAnimation(2,0,1.0f,100,handle, Easing::QuadraticIn);
                break;
            case 7:
                intAnimator.startAnimation(2,0,1.0f,100,handle, Easing::QuadraticOut);
                break;
            case 8:
                intAnimator.startAnimation(2,0,1.0f,100,handle, Easing::QuinticIn);
                break;
            case 9:
                intAnimator.startAnimation(2,0,1.0f,100,handle, Easing::QuinticOut);
                break;
            case 10:
                intAnimator.startAnimation(2,0,1.0f,100,handle, Easing::SinusoidalIn);
                break;
            case 11:
                intAnimator.startAnimation(2,0,1.0f,100,handle, Easing::SinusoidalOut);
                break;
            case 12:
                intAnimator.startAnimation(2,0,1.0f,100,handle, Easing::QuadraticInOut);
                break;
            case 13:
                intAnimator.startAnimation(2,0,1.0f,100,handle, Easing::CubicInOut);
                break;
            case 14:
                intAnimator.startAnimation(2,0,1.0f,100,handle, Easing::QuadraticInOut);
                break;
            case 15:
                intAnimator.startAnimation(2,0,1.0f,100,handle, Easing::QuinticInOut);
                break;
            case 16:
                intAnimator.startAnimation(2,0,1.0f,100,handle, Easing::SinusoidalInOut);
                break;
            case 17:
                intAnimator.startAnimation(2,0,1.0f,100,handle, Easing::ExponentialInOut);
                break;

        }
    }
}

class SolidRedColorSource : public ColorSource {
    public:
    SolidRedColorSource(uint32_t identifier) : ColorSource(identifier, 0) {}

    LightWeaver::RgbColor getColor(float progress) const {
        return LightWeaver::RgbColor(255,0,0);
    }

    ColorSource* clone() const {
        return new SolidRedColorSource(identifier);
    }
};

void setup()
{
    Serial.begin(115200);
    while(!Serial);

    LightWeaver::RgbColor colors[] = {
        LightWeaver::RgbColor(255, 000, 000),
        LightWeaver::RgbColor(255, 255, 000),
        LightWeaver::RgbColor(000, 255, 000),
        LightWeaver::RgbColor(000, 255, 255),
        LightWeaver::RgbColor(000, 000, 255),
        LightWeaver::RgbColor(255, 000, 255),
        LightWeaver::RgbColor(255, 000, 000)
    };

    lightWeaver.setup();
    lightWeaver.setColorSource(GradientColorSource(0x00000003, 3000, 7, colors));
    lightWeaver.setBrightness(32);

    intAnimator.startAnimation(2,0,1.0f,100,handle, Easing::Linear);
}

void loop()
{
    lightWeaver.loop();
    intAnimator.loop();
}