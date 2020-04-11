#pragma once
#include <memory>
#include "LightWeaverCore.h"
#include "LightWeaverPlugin.h"
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
    template <typename T_DRIVER, uint8_t MAXIMUM_PLUGINS = 0>
    class LightWeaverCoreImpl : public LightWeaverCore
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

        template <typename T>
        struct Transition<std::unique_ptr<T>> {
            private:
            void onAnimationTick(const AnimationParam& param) {
                    progress = param.easedProgress;
                }
            public:
            std::unique_ptr<T> originalValue;
            float progress;
            Animation animation;
            Transition(std::unique_ptr<T> originalValue):
                originalValue(std::move(originalValue)),
                progress(0.0f),
                animation(Animation(500, false, std::bind(&Transition<std::unique_ptr<T>>::onAnimationTick, this, std::placeholders::_1), Easing::QuadraticInOut)) {}
            Transition(const Transition<std::unique_ptr<T>>& other):
                Transition(other.originalValue) {}

        };

    // Todo: Compile time optimization of features
    //     static const bool supportsBrightness = static_cast<bool>(T_DRIVER::SupportedFeatures & SupportedFeature::BRIGHTNESS);
    //     static const bool supportsColor = static_cast<bool>(T_DRIVER::SupportedFeatures & SupportedFeature::COLOR);
    //     static const bool supportsAnimation = static_cast<bool>(T_DRIVER::SupportedFeatures & SupportedFeature::ANIMATION);
    //     static const bool supportsAddressable = static_cast<bool>(T_DRIVER::SupportedFeatures & SupportedFeature::ADDRESSABLE);

        T_DRIVER driver;
        uint8_t pixelCount;
        uint8_t pixelGroupSize;
        uint8_t brightness;

        LightWeaverPlugin** plugins = new LightWeaverPlugin*[MAXIMUM_PLUGINS];
        uint8_t currentPlugins = 0;

        static const int BACKGROUND_ANIMATION = 0;
        static const int COLOR_TRANSITION_ANIMATION = 1;
        static const int BRIGHTNESS_TRANSITION_ANIMATION = 2;
        Animator animator{3, Animator::AnimatorTimescale::MILLISECOND};

        std::unique_ptr<RgbColor[]> cachedColors;

        ColorSource* backgroundColorSource = nullptr;
        Transition<std::unique_ptr<RgbColor[]>>* colorTransition = nullptr;
        Transition<uint8_t>* brightnessTransition = nullptr;

        RgbColor getDisplayColor(uint8_t pixel = 0) {
            RgbColor backgroundColor = backgroundColorSource ? backgroundColorSource->getColor(pixel, pixelCount) : RgbaColor(0,0,0,255);
            if (colorTransition) {
                if (colorTransition->progress < 1.0f) {
                    return RgbColor::linearBlend(colorTransition->originalValue[pixel], backgroundColor, colorTransition->progress);
                } else if (colorTransition->progress == 1.0f) {
                    delete colorTransition;
                    colorTransition = nullptr;
                }
            }
            return backgroundColor;
        }

        uint8_t getDisplayBrightness() {
            if (brightnessTransition) {
                if (brightnessTransition->progress < 1.0f) {
                    return (brightness - brightnessTransition->originalValue) * brightnessTransition->progress + brightnessTransition->originalValue;
                } else if (brightnessTransition->progress == 1.0f) {
                    delete brightnessTransition;
                    brightnessTransition = nullptr;
                }
            }
            return brightness;
        }

    public:
        LightWeaverCoreImpl(uint8_t pixelCount, uint8_t pixelGroupSize, uint8_t brightness = 255) : 
            driver(T_DRIVER(pixelCount * pixelGroupSize)), 
            pixelCount(pixelCount),
            pixelGroupSize(pixelGroupSize),
            brightness(brightness),
            cachedColors(std::unique_ptr<RgbColor[]>(new RgbColor[pixelCount] )) {}
        virtual ~LightWeaverCoreImpl(){
            delete backgroundColorSource;
            backgroundColorSource = nullptr;
            delete colorTransition;
            colorTransition = nullptr;
            delete brightnessTransition;
            brightnessTransition = nullptr;
            for (uint8_t i = 0; i < MAXIMUM_PLUGINS; i++) {
                delete plugins[i];
                plugins[i] = nullptr;
            }
            delete[] plugins;
            plugins = nullptr;
        }

        template <typename PLUGIN>
        bool addPlugin() {
            if (currentPlugins >= MAXIMUM_PLUGINS) return false;
            plugins[currentPlugins++] = new PLUGIN(*this);
            return true;
        }

        const LightWeaverPlugin* getPluginOfType(const String& type) {
            for (uint8_t i = 0; i < MAXIMUM_PLUGINS; i++) {
                if (plugins[i] && plugins[i]->getType() == type) {
                    return plugins[i];
                }
            }
            return nullptr;
        }

        void setup()
        {
            for (uint8_t i = 0; i < MAXIMUM_PLUGINS; i++) {
                if (plugins[i]) {
                    plugins[i]->setup();
                }
            }

            driver.setup();
        }

        void loop()
        {
            for (uint8_t i = 0; i < MAXIMUM_PLUGINS; i++) {
                if (plugins[i]) {
                    plugins[i]->loop();
                }
            }

            animator.loop();
            for (uint8_t i = 0; i < pixelCount; i++) {
                cachedColors[i] = getDisplayColor(i);
                driver.setColor(cachedColors[i], i*pixelGroupSize, pixelGroupSize);
            }
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

        virtual void setBrightness(uint8_t b) {
            startBrightnessTransition();
            brightness = b;
        }

        virtual uint8_t getBrightness() {
            return this->brightness;
        }

        void startColorTransition() {
            std::unique_ptr<RgbColor[]> colors = std::unique_ptr<RgbColor[]>{ new RgbColor[pixelCount] };
            memcpy(colors.get(), cachedColors.get(), pixelCount * sizeof(RgbColor));

            animator.stopAnimation(COLOR_TRANSITION_ANIMATION);
            delete colorTransition;
            colorTransition = new Transition<std::unique_ptr<RgbColor[]>>(std::move(colors));
            animator.playAnimation(COLOR_TRANSITION_ANIMATION, colorTransition->animation);
        }

        virtual void clearColorSource() {
            startColorTransition();
            animator.stopAnimation(BACKGROUND_ANIMATION);
            delete backgroundColorSource;
            backgroundColorSource = nullptr;
        }

        virtual void setColorSource(const ColorSource& cs) {
            clearColorSource();

            backgroundColorSource = cs.clone();
            const Animation* anim = backgroundColorSource->getAnimation();
            animator.playAnimation(BACKGROUND_ANIMATION, anim);
        }

        virtual int getSupportedFeatures() {
            return T_DRIVER::SupportedFeatures;
        }
    };
};