#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoOta.h>
#include <LightWeaver.h>
#include <LightWeaver/LightWeaverWebPlugin.h>

namespace LightWeaver {
    class LightWeaverOta : public LightWeaverWebPlugin {
        private:
        public:
            static const String type;
            const String password;
            const String hostname;

            LightWeaverOta(LightWeaverCore& lightWeaver, String password, String hostname):
                LightWeaverWebPlugin(lightWeaver),
                password(password),
                hostname(hostname) {};
            LightWeaverOta(LightWeaverCore& lightWeaver):
                LightWeaverOta(lightWeaver, "lightweaver", "esp-lightweaver") {};

            virtual ~LightWeaverOta() {
            }

            virtual void setup() {
                ArduinoOTA.setHostname(hostname.c_str());
                ArduinoOTA.setPassword(password.c_str());
                ArduinoOTA.setPort(8266);
                ArduinoOTA.setRebootOnSuccess(true);
                LightWeaverWebPlugin::setup();
            }

            virtual void onWifiConnected() {
                ArduinoOTA.begin();
            }


            virtual void loop() {
                LightWeaverWebPlugin::loop();

                ArduinoOTA.handle();
            }

            virtual const String& getType() {
                return LightWeaverOta::type;
            }
    };

    const String LightWeaverOta::type = "OTA";
}