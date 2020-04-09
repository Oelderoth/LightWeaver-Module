#include <NeoPixelBrightnessBus.h>

#include "Driver.h"
#include "../Color.h"
#include "../Features.h"

namespace LightWeaver {
    /**
     * Driver class for controlling a strip of RGB LEDs using NeoPixelBus
     */
    template <typename T_FEATURE, typename T_METHOD, int GROUP_SIZE = 1>
    class NeoRgbDriver : Driver
    {
    private:
        NeoPixelBrightnessBus<T_FEATURE, T_METHOD> strip;
        uint16_t pixelCount;
    public:
        static const int SupportedFeatures = SupportedFeature::BRIGHTNESS | SupportedFeature::COLOR | SupportedFeature::ANIMATION | SupportedFeature::ADDRESSABLE;
        NeoRgbDriver(uint16_t pixelCount) : strip(NeoPixelBrightnessBus<T_FEATURE, T_METHOD>(pixelCount)), pixelCount(pixelCount) {};
        void setColor(RgbColor color) {
            for (int i = 0; i < pixelCount; i++) {
                strip.SetPixelColor(i, ::RgbColor(color.R, color.G, color.B));
            }
        };
        void setBrightness(uint8_t brightness) {
            strip.SetBrightness(brightness);
        };
        void setup(){
            strip.Begin();
        };
        void loop(){
            strip.Show();
        };
    };
}