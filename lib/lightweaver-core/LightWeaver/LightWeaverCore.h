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
        Animator::AnimationHandle* backgroundHandle = nullptr;


        RgbColor getDisplayColor() {
            RgbColor backgroundColor = backgroundColorSource ? backgroundColorSource->getColor() : RgbaColor(0,0,0,255);
            return backgroundColor;
        }

        uint8_t getDisplayBrightness() {
            return brightness;
        }

    public:
        LightWeaverCore(uint8_t pixelCount, uint8_t brightness = 255) : driver(T_DRIVER(pixelCount)), brightness(brightness) {}
        ~LightWeaverCore(){
            delete backgroundColorSource;
            backgroundColorSource = nullptr;
            delete backgroundHandle;
            backgroundHandle = nullptr;
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
            brightness = b;
        }

        uint8_t getBrightness() {
            return this->brightness;
        }

        void clearColorSource() {
            delete backgroundColorSource;
            animator.stopAnimation(backgroundHandle);
            delete backgroundHandle;
        }

        void setColorSource(const ColorSource& cs) {
            clearColorSource();
            backgroundColorSource = cs.clone();
            const Animation* anim = backgroundColorSource->getAnimation();
            backgroundHandle = animator.playAnimation(anim);
        }
    };
};