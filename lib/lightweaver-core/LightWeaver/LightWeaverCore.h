#pragma once

#include "animation/Animator.h"
#include "internal/ColorSource.h"

#include "Features.h"

namespace LightWeaver {
    /**
     * The core class of LightWeaver
     * Responsible for orchestrating the setup and loop steps of all necessary components
     * and delegating color updates to the provided driver class
     * 
     * The core maintains a primary ColorSource (which does not support transparency) to 
     * determine the final display color and handles smoothly transitioning colors when 
     * the ColorSource is changed. Brightness changes are also handled via smooth transitions.
     * 
     * 
     */
    struct ColorSourceAnimation {
        private:
            RgbaColor cachedColor;
            bool isDirty;
        public:
            const ColorSource* colorSource;
            float progress;
            uint16_t iterations;

            ColorSourceAnimation(const ColorSource* colorSource, float progress): 
                cachedColor(RgbaColor(0,0,0,255)), 
                isDirty(true), 
                colorSource(colorSource ? colorSource->clone() : nullptr), 
                progress(progress), 
                iterations(0) {};
            ColorSourceAnimation(const ColorSourceAnimation& anim): 
                cachedColor(anim.cachedColor),
                isDirty(anim.isDirty),
                colorSource(anim.colorSource ? anim.colorSource->clone() : nullptr),
                progress(anim.progress),
                iterations(anim.iterations) {};
            ColorSourceAnimation(): ColorSourceAnimation(nullptr, 0.0f) {};
            ~ColorSourceAnimation() {
                delete colorSource;
                colorSource = nullptr;
            };

            RgbaColor getColor() {
                if (isDirty) {
                    if (colorSource) {
                        cachedColor = colorSource->getColor(progress);
                    }
                    isDirty = false;
                }
                return cachedColor;
            }

            void setProgress(float p) {
                progress = p;
                isDirty = true;
            }
    };

    template<typename T>
    struct TransitionAnimation {
        T originalValue;
        float progress;
        TransitionAnimation(T originalValue, float progress) : originalValue(originalValue), progress(progress) {};
    };

    template <typename T_DRIVER, uint8_t MAXIMUM_EFFECTS=4>
    class LightWeaverCore
    {
    private:
    // Todo: Compile time optimization of features
    //     static const bool supportsBrightness = static_cast<bool>(T_DRIVER::SupportedFeatures & SupportedFeature::BRIGHTNESS);
    //     static const bool supportsColor = static_cast<bool>(T_DRIVER::SupportedFeatures & SupportedFeature::COLOR);
    //     static const bool supportsAnimation = static_cast<bool>(T_DRIVER::SupportedFeatures & SupportedFeature::ANIMATION);
    //     static const bool supportsAddressable = static_cast<bool>(T_DRIVER::SupportedFeatures & SupportedFeature::ADDRESSABLE);

        T_DRIVER driver;
        uint8_t brightness;

        static const int BACKGROUND_ANIMATION = 0;
        static const int COLOR_TRANSITION_ANIMATION = 1;
        static const int BRIGHTNESS_TRANSITION_ANIMATION = 2;
        Animator animator{3, AnimatorTimescale::MILLISECOND};

        ColorSourceAnimation* backgroundAnimation = nullptr;
        TransitionAnimation<RgbColor>* colorTransitionAnimation = nullptr;
        TransitionAnimation<uint8_t>* brightnessTransitionAnimation = nullptr;


        RgbColor getDisplayColor() {
            RgbColor backgroundColor = backgroundAnimation ? backgroundAnimation->getColor() : RgbaColor(0,0,0,0);
            if (colorTransitionAnimation) {
                return RgbColor::linearBlend(colorTransitionAnimation->originalValue, backgroundColor, colorTransitionAnimation->progress);
            }
            return backgroundColor;
        }

        uint8_t getDisplayBrightness() {
            if (brightnessTransitionAnimation) {
                // return (brightness - brightnessTransitionAnimation->originalValue) * brightnessTransitionAnimation->progress + brightnessTransitionAnimation->originalValue;
                return brightness;
            }
            return brightness;
        }

    public:
        LightWeaverCore(uint8_t pixelCount, uint8_t brightness = 255) : driver(T_DRIVER(pixelCount)), brightness(brightness) {}
        ~LightWeaverCore(){
            delete backgroundAnimation;
            backgroundAnimation = nullptr;
            delete colorTransitionAnimation;
            colorTransitionAnimation = nullptr;
            delete brightnessTransitionAnimation;
            brightnessTransitionAnimation = nullptr;
        }

        void setup()
        {
            driver.setup();
        }

        void loop()
        {
            animator.loop();
            driver.setColor(getDisplayColor());
            driver.setBrightness(getDisplayBrightness());
            driver.loop();
        }

        void setBrightness(uint8_t b) {
            uint8_t currentBrightness = getDisplayBrightness();
            delete brightnessTransitionAnimation;
            brightnessTransitionAnimation = new TransitionAnimation<uint8_t>(currentBrightness, 0.0f);

            brightness = b;
            
            animator.startAnimation(BRIGHTNESS_TRANSITION_ANIMATION, 500, false, [this](const AnimationParam& param){
                brightnessTransitionAnimation->progress = param.easedProgress;
                if (param.state == AnimationState::Completed) {
                    delete brightnessTransitionAnimation;
                    brightnessTransitionAnimation = nullptr;
                }
            }, Easing::QuadraticInOut);
        }

        uint8_t getBrightness() {
            return this->brightness;
        }

        void setColorSource(const ColorSource& cs) {
            RgbColor currentColor = getDisplayColor();
            
            delete colorTransitionAnimation;
            colorTransitionAnimation = new TransitionAnimation<RgbColor>(currentColor, 0.0f);

            delete backgroundAnimation;
            backgroundAnimation = new ColorSourceAnimation(&cs, 0.0f);

            uint16_t duration = backgroundAnimation->colorSource->duration;
            if (duration > 0) {
                animator.startAnimation(BACKGROUND_ANIMATION, duration, true, [this](const AnimationParam& param) {
                    Serial.println(param.easedProgress);
                    backgroundAnimation->setProgress(param.easedProgress);
                }, Easing::Linear);                
            }

            animator.startAnimation(COLOR_TRANSITION_ANIMATION, 500, false, [this](const AnimationParam& param){
                colorTransitionAnimation->progress = param.easedProgress;
                if (param.state == AnimationState::Completed) {
                    delete colorTransitionAnimation;
                    colorTransitionAnimation = nullptr;
                }
            }, Easing::QuadraticInOut);
        }
    };
};