#pragma once
#include <Arduino.h>

#define WIFI_TIMEOUT 60000
#define JSON_MAX_SIZE 8192
#define EVENT_BUFFER_SIZE 512
#define SETTINGS_FILE "/settings.json"
#define OTA_PASSWORD "ESPturismo"
#define RAMP_UP_TIME 50
#define RAMP_DOWN_TIME 10
#define PWM_FREQ 500
#define PIN_LED_WIFI        LED_BUILTIN
#define PIN_LED_TELEMETRY   LED_BUILTIN

#ifdef ARDUINO_LOLIN_S2_MINI
    #define PIN_DECEL1 2
    #define PIN_DECEL2 4
    #define PIN_ACCEL1 6
    #define PIN_ACCEL2 8
#elif ARDUINO_ESP32_DEV
    #define PIN_ACCEL1 25
    #define PIN_ACCEL2 26
    #define PIN_DECEL1 32
    #define PIN_DECEL2 33
#endif