#include <NeoPixelBrightnessBus.h>

#include "internal/Features.h";

namespace LightWeaver {
    /**
     * Driver class for controlling a strip of RGBW SK6182 LEDs using NeoPixelBus
     */
    template <int GROUP_SIZE>
    class NeoDriverSK6812_RGBW
    {
    private:
        NeoPixelBrightnessBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod> strip;

    public:
        NeoDriverSK6812_RGBW(uint16_t pixelCount) : strip(NeoPixelBrightnessBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod>(pixelCount)){};

        static SupportedFeature supportedFeatures() {
            return SupportedFeature::BRIGHTNESS | SupportedFeature::COLOR | SupportedFeature::ANIMATION | SupportedFeature::ADDRESSABLE;
        }

        void setup(){};
        void loop(){};
    };
};