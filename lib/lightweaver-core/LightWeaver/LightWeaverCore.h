#pragma once

#include "ColorSource.h"
#include "Features.h"
#include "animation/Animator.h"


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
    template <typename T_DRIVER, uint8_t MAXIMUM_EFFECTS=4>
    class LightWeaverCore
    {
    private:
        template <typename T>
        struct Transition {
            private:
                void onAnimationTick(const AnimationParam& param) {
                    progress = param.easedProgress;
                }
            public:
            T originalValue;
            float progress;
            Animation animation;
            Transition(T originalValue): 
                originalValue(originalValue), 
                progress(0.0f),
                animation(Animation(500, false, std::bind(&Transition<T>::onAnimationTick, this, std::placeholders::_1), Easing::QuadraticInOut)) {}

            void startTransition(Animator& animator, uint16_t animationIndex) {
                animator.playAnimation(animationIndex, animation);
            }
        };

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
        Animator animator{3, Animator::AnimatorTimescale::MILLISECOND};

        ColorSource* backgroundColorSource = nullptr;
        Transition<RgbColor>* colorTransition = nullptr;
        Transition<uint8_t>* brightnessTransition = nullptr;

        RgbColor getDisplayColor() {
            RgbColor backgroundColor = backgroundColorSource ? backgroundColorSource->getColor() : RgbaColor(0,0,0,255);
            if (colorTransition && colorTransition->progress < 1.0f) {
                return RgbColor::linearBlend(colorTransition->originalValue, backgroundColor, colorTransition->progress);
            }
            return backgroundColor;
        }

        uint8_t getDisplayBrightness() {
            if (brightnessTransition && brightnessTransition->progress < 1.0f) {
                return (brightness - brightnessTransition->originalValue) * brightnessTransition->progress + brightnessTransition->originalValue;
            }
            return brightness;
        }

    public:
        LightWeaverCore(uint8_t pixelCount, uint8_t brightness = 255) : driver(T_DRIVER(pixelCount)), brightness(brightness) {}
        ~LightWeaverCore(){
            delete backgroundColorSource;
            backgroundColorSource = nullptr;
            delete colorTransition;
            colorTransition = nullptr;
            delete brightnessTransition;
            brightnessTransition = nullptr;
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

        void startBrightnessTransition() {
            uint8_t brightness = getDisplayBrightness();
            animator.stopAnimation(BRIGHTNESS_TRANSITION_ANIMATION);
            delete brightnessTransition;
            brightnessTransition = new Transition<uint8_t>(brightness);
            animator.playAnimation(BRIGHTNESS_TRANSITION_ANIMATION, brightnessTransition->animation);
        }

        void setBrightness(uint8_t b) {
            startBrightnessTransition();
            brightness = b;
        }

        uint8_t getBrightness() {
            return this->brightness;
        }

        void clearColorSource() {
            animator.stopAnimation(BACKGROUND_ANIMATION);
            delete backgroundColorSource;
        }

        void startColorTransition() {
            RgbColor color = getDisplayColor();
            animator.stopAnimation(COLOR_TRANSITION_ANIMATION);
            delete colorTransition;
            colorTransition = new Transition<RgbColor>(color);
            animator.playAnimation(COLOR_TRANSITION_ANIMATION, colorTransition->animation);
        }

        void setColorSource(const ColorSource& cs) {
            startColorTransition();
            clearColorSource();

            backgroundColorSource = cs.clone();
            const Animation* anim = backgroundColorSource->getAnimation();
            animator.playAnimation(BACKGROUND_ANIMATION, anim);
        }
    };
};