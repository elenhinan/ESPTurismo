#pragma once
#include <Arduino.h>
#include <salsa20.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <ArduinoOTA.h>
#include "settings.h"
#include "config.h"
#include "telemetry.h"
#include "webserver.h"

const char* settings_file = SETTINGS_FILE;

void setup_pins();
void setup_wifi();
void setup_ota();
void set_outputs();