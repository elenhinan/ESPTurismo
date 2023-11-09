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
        void routes();
    public:
        void begin();
};

extern WebServer webserver;