#pragma once
#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "config.h"

/*
struct parameter {
    char*   name;          // parameter name
    uint8_t pin;         // output pin
    uint8_t output;      // output pwm value (0-255)
    char*   trig_variable; // gran turismo variable to trigger on
    bool    trig_operator; // 0: less or equal, 1: greater or equal
    float   trig_value;    // value to compare to
    bool    triggered;
};
*/

enum mode : unsigned int {
    ANY_WHEEL,
    AVG_WHEEL,
    TCS,
    ASM,
    REV_LIMIT 
};

class Settings {
    public:
        char wifi_ssid[32];
        char wifi_pwd[32];
        char hostname[32];
        char playstation[32];

        uint8_t     decel_pin1;
        uint8_t     decel_pin2;
        float       decel_output;
        float       decel_threshold;
        enum mode   decel_mode;
        float       decel_min_pedal;
        float       decel_min_speed;

        uint8_t     accel_pin1;
        uint8_t     accel_pin2;
        float       accel_output;
        float       accel_threshold;
        enum mode   accel_mode;
        float       accel_min_pedal;
        float       accel_min_speed;

        void load(const char* filename);
        void save(const char* filename);
        void from_json(JsonObject &root);
        void to_json(JsonObject &root);
};

extern Settings settings;