#pragma once
#include <Arduino.h>
#include <salsa20.h>
#include <WiFi.h>
#include <LittleFS.h>
#include "settings.h"
#include "config.h"
#include "telemetry.h"
#include "webserver.h"

const char* settings_file = SETTINGS_FILE;

void setup_ports();
void setup_wifi();