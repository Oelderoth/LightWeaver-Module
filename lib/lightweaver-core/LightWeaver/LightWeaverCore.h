#pragma once

#include "internal/Animator.h"
#include "internal/ColorSource.h"
#include "internal/Features.h"

namespace LightWeaver {
    /**
     * The core class of LightWeaver
     * Responsible for orchestrating the setup and loop steps of all necessary components
     * and delegating color updates to the provided driver class
     */

    template <typename T_DRIVER>
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
        ColorSource* colorSource;
    public:
        LightWeaverCore(int pixelCount) : driver(T_DRIVER(pixelCount)) {}
        ~LightWeaverCore(){}

        void setup()
        {
            driver.setup();
        }

        void loop()
        {
            RgbColor color = colorSource->getColor(0);
            driver.setColor(color);
            driver.setBrightness(255);
            driver.loop();
        }

        void setBrightness(uint8_t b) {
            brightness = b;
        }

        uint8_t getBrightness() {
            return this.brightness;
        }

        void setColorSource(ColorSource& cs) {
            colorSource = cs.clone();
        }
        ColorSource& getColorSource() {
            return colorSource;
        }
    };
};