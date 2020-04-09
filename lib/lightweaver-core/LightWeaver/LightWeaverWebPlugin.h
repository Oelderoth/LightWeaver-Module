#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <LightWeaver/LightWeaverPlugin.h>

namespace LightWeaver {
    /**
     * Utility class to simplify plugins that need to run code when the device
     * is connected to/disconnected from WiFi. Since that may not happen at startup
     * the plugin should not expect that WiFi is connect during setup()
     */
    class LightWeaverWebPlugin : public LightWeaverPlugin {
        private:
            bool _isConnected = false;
        public:
            LightWeaverWebPlugin(LightWeaverCore& lightWeaver):
                LightWeaverPlugin(lightWeaver) {};
            virtual ~LightWeaverWebPlugin() {}

            virtual void setup() {
                if (WiFi.status() == WL_CONNECTED) {
                    _isConnected = true;
                    onWifiConnected();
                }
            }

            virtual void loop() {
                wl_status_t status = WiFi.status();
                if (!_isConnected && status == WL_CONNECTED) {
                    _isConnected = true;
                    onWifiConnected();
                } else if (_isConnected && status != WL_CONNECTED) {
                    _isConnected = false;
                    onWifiDisconnected();
                }
            }

            virtual void onWifiConnected() {};
            virtual void onWifiDisconnected() {};
    };
}