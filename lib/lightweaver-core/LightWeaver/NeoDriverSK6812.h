#include <NeoPixelBrightnessBus.h>

#include "internal/Color.h"
#include "internal/Features.h"

namespace LightWeaver {
    /**
     * Driver class for controlling a strip of RGBW SK6182 LEDs using NeoPixelBus
     */
    template <int GROUP_SIZE>
    class NeoDriverSK6812_RGBW
    {
    private:
        NeoPixelBrightnessBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod> strip;
        uint16_t pixelCount;
    public:
        static const int SupportedFeatures = SupportedFeature::BRIGHTNESS | SupportedFeature::COLOR | SupportedFeature::ANIMATION | SupportedFeature::ADDRESSABLE;
        NeoDriverSK6812_RGBW(uint16_t pixelCount) : strip(NeoPixelBrightnessBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod>(pixelCount)), pixelCount(pixelCount) {};
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

    class NoopDriver {
        public:
        static const int SupportedFeatures = 0;
        NoopDriver(uint16_t pixelCount) {};
        void setup(){};
        void loop(){}
    };
}