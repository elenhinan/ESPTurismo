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

        unsigned int   decel_pin;
        unsigned int   decel_output;
        double     decel_threshold;
        enum mode decel_mode;
        unsigned int   decel_min_pedal;
        double     decel_min_speed;

        unsigned int   accel_pin;
        unsigned int   accel_output;
        double     accel_threshold;
        enum mode accel_mode;
        unsigned int   accel_min_pedal;
        double     accel_min_speed;

        void load(char* filename);
        void save(char* filename);
        void from_json(JsonObject &root);
        void to_json(JsonObject &root);
};

extern Settings settings;