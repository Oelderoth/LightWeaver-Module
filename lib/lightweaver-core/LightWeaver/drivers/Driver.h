#include <NeoPixelBrightnessBus.h>

#include "../Color.h"

#include "../Features.h"

namespace LightWeaver {
    /**
     * Driver class for controlling a strip of RGBW SK6182 LEDs using NeoPixelBus
     */
    class Driver {
        public:
        virtual void setup() = 0;
        virtual void setColor(RgbColor color) = 0;
        virtual void setBrightness(uint8_t brightness)  = 0;
        virtual void loop() = 0;
    };

    class NoopDriver : Driver {
        public:
        static const int SupportedFeatures = 0;
        NoopDriver(uint16_t pixelCount) {};
        void setup(){};
        void setColor(RgbColor color) {};
        void setBrightness(uint8_t brightness) {};
        void loop(){}
    };
}