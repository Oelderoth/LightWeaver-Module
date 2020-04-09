#pragma once
#include <Arduino.h>
#include "ColorSource.h"
#include "Features.h"

namespace LightWeaver {
    class LightWeaverPlugin;
    class LightWeaverCore {
        public:
            virtual ~LightWeaverCore() {};

            virtual void setBrightness(uint8_t brightness) = 0;
            virtual uint8_t getBrightness() = 0;
            virtual void setColorSource(const ColorSource& cs) = 0;
            virtual void clearColorSource() = 0;
            virtual int getSupportedFeatures() = 0;
            
            virtual const LightWeaverPlugin* getPluginOfType(const String& type) = 0;
    };
}