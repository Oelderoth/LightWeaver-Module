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
            strip.ClearTo(::RgbColor(color.R, color.G, color.B));
        };
        void setColor(RgbColor color, uint8_t index, uint8_t length) {
            ::RgbColor rgb = ::RgbColor(color.R, color.G, color.B);
            if (length == 1) {
                strip.SetPixelColor(index, rgb);
            } else {
                strip.ClearTo(rgb, index, index + length - 1);
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