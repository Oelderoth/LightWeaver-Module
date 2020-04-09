#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <LightWeaver/LightWeaverPlugin.h>
#include <LightWeaver/colorSources/FadeColorSource.h>

#include "AsyncWifiManager.h"

namespace LightWeaver {
    class LightWeaverWifi : public LightWeaverPlugin {
        private:
            AsyncWifiManager wifiManager;

            void playIdleAnimation() {
                lightWeaver->setColorSource(LightWeaver::FadeColorSource{0x0001,
                    LightWeaver::RgbColor(128,128,128),
                    LightWeaver::RgbColor(255,255,255),
                    5000, true, Easing::Mirror(Easing::QuadraticInOut)});
            }
        public:
            static const String type;

            LightWeaverWifi(LightWeaverCore& lightWeaver):
                LightWeaverPlugin(lightWeaver) {};
            virtual ~LightWeaverWifi() {
            }

            virtual void setup() {
                wifiManager.setOnAPStartedHandler([this]() {
                    playIdleAnimation();
                });

                wifiManager.setOnWifiConnectedHandler([this]() {
                    wifiManager.end();
                    lightWeaver->clearColorSource();
                });

                wifiManager.begin("Lightweaver-"+String(ESP.getChipId()));
            }

            virtual void loop() {
                wifiManager.loop();
            }

            virtual const String& getType() {
                return LightWeaverWifi::type;
            }
    };

    const String LightWeaverWifi::type = "WIFI";
}