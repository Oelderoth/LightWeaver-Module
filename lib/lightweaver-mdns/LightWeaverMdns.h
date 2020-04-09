#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <LightWeaver.h>
#include <LightWeaver/Features.h>
#include <LightWeaver/LightWeaverWebPlugin.h>
#include <LightWeaver/util/StringListBuilder.h>

namespace LightWeaver {
    class LightWeaverMdns : public LightWeaverWebPlugin {
        private:

            String getFeatureString(int features) {
                StringListBuilder builder{", ", ""};
                if (features & SupportedFeature::BRIGHTNESS) {
                    builder += "BRIGHTNESS";
                }
                if (features & SupportedFeature::COLOR) {
                    builder += "COLOR";
                }
                if (features & SupportedFeature::ANIMATION) {
                    builder += "ANIMATION";
                }
                if (features & SupportedFeature::ADDRESSABLE) {
                    builder += "ADDRESSABLE";
                }
                return builder;
            }
        public:
            static const String type;

            LightWeaverMdns(LightWeaverCore& lightWeaver):
                LightWeaverWebPlugin(lightWeaver) {};
            virtual ~LightWeaverMdns() {
            }

            virtual void onWifiConnected() {
                MDNS.begin("esp-lightweaver");
                MDNS.addService("lightweaver","tcp",80);
                MDNS.addServiceTxt("lightweaver", "tcp", "device-id", String(ESP.getChipId()));
                MDNS.addServiceTxt("lightweaver", "tcp", "firmware-version", LIGHTWEAVER_VERSION);
                MDNS.addServiceTxt("lightweaver", "tcp", "supported-features", getFeatureString(lightWeaver->getSupportedFeatures()));
                
                if (lightWeaver->getPluginOfType(LightWeaverHttpServer::type)) {
                    MDNS.addServiceTxt("lightweaver", "tcp", "http-api-version", LightWeaverHttpServer::apiVersion);
                    MDNS.addService("http","tcp",LightWeaverHttpServer::port);
                    MDNS.addServiceTxt("http", "tcp", "path", LightWeaverHttpServer::rootPath);
                }
            }

            virtual void onWifiDisconnected() {
                MDNS.end();
            }

            virtual void loop() {
                LightWeaverWebPlugin::loop();

                MDNS.update();
            }

            virtual const String& getType() {
                return LightWeaverMdns::type;
            }
    };

    const String LightWeaverMdns::type = "MDNS";
}