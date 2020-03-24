#include <NeoPixelBrightnessBus.h>

#include "internal/Features.h";

template <int GROUP_SIZE>
class NeoDriverSK6812
{
private:
    NeoPixelBrightnessBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod> strip;

public:
    NeoDriverSK6812(uint16_t pixelCount) : strip(NeoPixelBrightnessBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod>(pixelCount)){};

    static SupportedFeature supportedFeatures() {
        return SupportedFeature::BRIGHTNESS | SupportedFeature::COLOR | SupportedFeature::ANIMATION | SupportedFeature::ADDRESSABLE;
    }

    void setup(){};
    void loop(){};
};