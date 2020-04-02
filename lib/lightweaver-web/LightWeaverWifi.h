#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <LightWeaver/LightWeaverPlugin.h>

#include "AsyncWifiManager.h"
#include "FadeColorSource.h"

namespace LightWeaver {
    class LightWeaverWifi : public LightWeaverPlugin {
        private:
            AsyncWifiManager wifiManager;

            void playIdleAnimation() {
                lightWeaver->setColorSource(LightWeaverServer::FadeColorSource{0x0001,
                    LightWeaver::RgbColor(128,128,128),
                    LightWeaver::RgbColor(255,255,255),
                    5000, true, Easing::Mirror(Easing::QuadraticInOut)});
            }
        public:
            LightWeaverWifi(LightWeaverCore& lightWeaver):
                LightWeaverPlugin(lightWeaver) {};
            virtual ~LightWeaverWifi() {
            }

            virtual void setup() {
                WiFi.disconnect();
                wifiManager.setOnAPStartedHandler([this]() {
                    playIdleAnimation();
                });

                wifiManager.setOnWifiConnectedHandler([this]() {
                    wifiManager.end();
                });

                wifiManager.begin("Lightweaver-"+String(ESP.getChipId()));
            }

            virtual void loop() {
                wifiManager.loop();
            }
    };
}