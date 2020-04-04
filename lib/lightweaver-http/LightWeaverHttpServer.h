#pragma once
#include <Arduino.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <LightWeaver/colorSources/SolidColorSource.h>
#include <LightWeaver/LightWeaverPlugin.h>

#include "internal/ColorSourceDeserializer.h"

namespace LightWeaver {
    class LightWeaverHttpServer : public LightWeaverPlugin {
        private:
            AsyncWebServer server{80};
            bool isServerStarted = false;
            void startServer() {
                server.begin();
                isServerStarted = true;
            }
        public:
            LightWeaverHttpServer(LightWeaverCore& lightWeaver): LightWeaverPlugin(lightWeaver) {
            }
            virtual ~LightWeaverHttpServer() {
                server.end();
            }
            
            virtual void setup() {
                server.on("/lightWeaver/alive",HTTP_GET,[](AsyncWebServerRequest *request) {
                    request->send(200,"text/html","OK");
                });

                server.addHandler(new AsyncCallbackJsonWebHandler("/setColor", [this](AsyncWebServerRequest *request, JsonVariant &json) {
                    uint8_t red = json.getMember("red").as<uint8_t>();
                    uint8_t green = json.getMember("green").as<uint8_t>();
                    uint8_t blue = json.getMember("blue").as<uint8_t>();
                    lightWeaver->setColorSource(SolidColorSource(0x00000000,RgbColor(red, green, blue)));
                    request->send(204);
                }));

                server.addHandler(new AsyncCallbackJsonWebHandler("/setColorSource", [this](AsyncWebServerRequest *request, JsonVariant &json) {
                    ColorSourceDeserializer::Result result = ColorSourceDeserializer::deserialize(json);
                    
                    if (!result) {
                        request->send(422,"text/json","{\"error\":\"" + result.error + "\"}");
                    } else {
                        if (result.value) {
                            lightWeaver->setColorSource(*(result.value));
                        }
                        request->send(204);
                    }
                }));

                server.on("/clearColorSource",[this](AsyncWebServerRequest* request) {
                    lightWeaver->clearColorSource();
                    request->send(204);
                });
            }

            virtual void loop() {
                if (!isServerStarted && WiFi.status() == WL_CONNECTED) {
                    startServer();
                }
            }
    };
}