#pragma once
#include <Arduino.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <LightWeaver/colorSources/SolidColorSource.h>
#include <LightWeaver/LightWeaverWebPlugin.h>

#include "internal/ColorSourceDeserializer.h"

namespace LightWeaver {
    class LightWeaverHttpServer : public LightWeaverWebPlugin {
        public:
            static const String type;
            static const String apiVersion;
            static const String rootPath;
            static const uint16_t port = 80;
        private:
            AsyncWebServer server{80};
            bool isServerStarted = false;
            void startServer() {
                server.begin();
                isServerStarted = true;
            }
        public:
            LightWeaverHttpServer(LightWeaverCore& lightWeaver): LightWeaverWebPlugin(lightWeaver) {
            }
            virtual ~LightWeaverHttpServer() {
                server.end();
            }
            
            virtual void setup() {
                server.on((rootPath + "/alive").c_str(),HTTP_GET,[](AsyncWebServerRequest *request) {
                    request->send(200,"text/html","OK");
                });

                server.addHandler(new AsyncCallbackJsonWebHandler((rootPath + "/setColorSource").c_str(), [this](AsyncWebServerRequest *request, JsonVariant &json) {
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

                server.on((rootPath + "/clearColorSource").c_str(), [this](AsyncWebServerRequest* request) {
                    lightWeaver->clearColorSource();
                    request->send(204);
                });

                 server.addHandler(new AsyncCallbackJsonWebHandler((rootPath + "/setBrightness").c_str(), [this](AsyncWebServerRequest *request, JsonVariant &json) {
                    JsonVariant brightness = json["brightness"];
                    if (brightness.isNull()) {
                        request->send(422,"text/json","{\"error\":\"Required fields missing: brightness\"}");
                    } if (!brightness.is<uint8_t>()) {
                        request->send(422,"text/json","{\"error\":\"Invalid value for fields: brightness\"}");
                    } else {
                        lightWeaver->setBrightness(brightness);
                        request->send(204);
                    }
                }));

                

                server.onNotFound([](AsyncWebServerRequest* request) {
                    request->send(404);
                });


                LightWeaverWebPlugin::setup();
            }

            virtual void onWifiConnected() {
                startServer();
            }

            virtual const String& getType() {
                return LightWeaverHttpServer::type;
            }
    };

    const String LightWeaverHttpServer::type = "HTTP_SERVER";
    const String LightWeaverHttpServer::apiVersion = "0.1.0";
    const String LightWeaverHttpServer::rootPath = "/lightWeaver";
}