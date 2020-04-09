#include "AsyncWifiManager.h"

namespace LightWeaver {
    String AsyncWifiManager::templateProcessor(const String& var) {
        if (var == "SSID")
            return WiFi.SSID();
        if (var == "PSK")
            return WiFi.psk();
        if (var == "CHIP_ID")
            return String(ESP.getChipId());
        if (var == "FLASH_CHIP_ID")
            return String(ESP.getFlashChipId());
        if (var == "IDE_FLASH_SIZE")
            return String(ESP.getFlashChipSize());
        if (var == "REAL_FLASH_SIZE")
            return String(ESP.getFlashChipRealSize());
        if (var == "SOFTAP_IP")
            return WiFi.softAPIP().toString();
        if (var == "SOFTAP_MAC")
            return WiFi.softAPmacAddress();
        if (var == "STATION_MAC")
            return WiFi.macAddress();
        return String();
    }

    void AsyncWifiManager::handleCaptivePortal(AsyncWebServerRequest* request) {
        request->redirect("/");
    }

    bool AsyncWifiManager::isConnectedToWifi() {
        if (WiFi.waitForConnectResult() == WL_CONNECTED) {
            if (onWifiConnectedHandler) onWifiConnectedHandler();
            return true;
        }
        return false;
    }

    bool AsyncWifiManager::connectToWifi() {
        WiFi.begin();
        return false;
    };

    bool AsyncWifiManager::connectToWifi(String& ssid, String& psk) {
        WiFi.begin(ssid, psk);
        return false;
    };

    void AsyncWifiManager::begin(String softApName) {
        connectToWifi();
        if (isConnectedToWifi()) {
            return;
        }

        SPIFFS.begin();
        WiFi.softAPConfig(apIP, apIP, netMsk);
        WiFi.softAP(softApName);
        if (onAPStartedHandler) {
            onAPStartedHandler();
        }

        startCaptivePortal();
    };

    void AsyncWifiManager::startCaptivePortal() {
        dnsServer.setTTL(600);
        dnsServer.setErrorReplyCode(AsyncDNSReplyCode::ServerFailure);

        dnsServer.start(DNS_PORT, "*", apIP);
        
        server.on("/saveWifi",HTTP_POST,[this](AsyncWebServerRequest *request) {
            if (request->hasParam("ssid", true) && request->hasParam("psk", true)) {
                Serial.println("Connecting");
                String ssid = request->getParam("ssid", true)->value();
                String psk = request->getParam("psk", true)->value();
                connectToWifi(ssid,psk);
            }
            request->redirect("/connecting.html");
        });

        // Serve the CSS file w/out a template processor to avoid mangling percentages
        server.serveStatic("/style.css", SPIFFS, "/www/style.css");
        server.serveStatic("/", SPIFFS, "/www/")
            .setDefaultFile("index.html")
            .setTemplateProcessor(std::bind(&AsyncWifiManager::templateProcessor,this,std::placeholders::_1));

        server.onNotFound(std::bind(&AsyncWifiManager::handleCaptivePortal, this, std::placeholders::_1));
        
        server.begin();
    }

    void AsyncWifiManager::end() {
        dnsServer.stop();
        server.end();
        WiFi.softAPdisconnect(true);
    };

    void AsyncWifiManager::setOnAPStartedHandler(OnAPStartedHandler handler) {
        onAPStartedHandler = handler;
    };

    void AsyncWifiManager::setOnWifiConnectedHandler(OnWifiConnectedHandler handler) {
        onWifiConnectedHandler = handler;
    };
    
    void AsyncWifiManager::loop() {
        if (!hasFiredWifiCallbacks && WiFi.status() == WL_CONNECTED) {
            if (onWifiConnectedHandler) onWifiConnectedHandler();
            hasFiredWifiCallbacks = true;        
        }
    };
}