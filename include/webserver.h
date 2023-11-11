#pragma once
#include <Arduino.h>
#include <ESPmDNS.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "settings.h"
#include "telemetry.h"

class WebServer {
    private:
        void setup_routes();
        void setup_events();
    public:
        void begin();
        void send_event(const char* data, const char* name);
};

extern WebServer webserver;