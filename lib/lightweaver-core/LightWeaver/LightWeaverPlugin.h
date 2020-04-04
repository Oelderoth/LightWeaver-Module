#pragma once

#include "LightWeaverCore.h"

namespace LightWeaver {
    class LightWeaverPlugin {
        protected:
        LightWeaverCore * const lightWeaver;

        public:
        LightWeaverPlugin(): lightWeaver(nullptr) {};
        LightWeaverPlugin(LightWeaverCore& lightWeaver):
            lightWeaver(&lightWeaver) {};
        
        virtual ~LightWeaverPlugin() {};
        virtual void setup() {};
        virtual void loop() {};
    };
}