#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncDNSServer.h>


namespace LightWeaver {
    class AsyncWifiManager {
        typedef std::function<void()> OnAPStartedHandler;
        typedef std::function<void()> OnWifiConnectedHandler;

        bool hasFiredWifiCallbacks = false;

        const byte DNS_PORT = 53;
        const IPAddress apIP{192,168,0,1};
        const IPAddress netMsk{255,255,255,0};

        AsyncWebServer server{80};
        AsyncDNSServer dnsServer;
        OnAPStartedHandler onAPStartedHandler = nullptr;
        OnWifiConnectedHandler onWifiConnectedHandler = nullptr;

        void startCaptivePortal();
        void handleCaptivePortal(AsyncWebServerRequest*);
        String templateProcessor(const String& var);
        bool isConnectedToWifi();

        public:
            bool connectToWifi();
            bool connectToWifi(String& ssid, String& psk);
            void begin(String softApName);
            void loop();
            void end();
            
            void setOnAPStartedHandler(OnAPStartedHandler handler);
            void setOnWifiConnectedHandler(OnWifiConnectedHandler handler);
    };
}