#pragma once
#include <Arduino.h>
#include "settings.h"
#include "config.h"

enum rumble_state: uint8_t {
    RUMBLE_STOPPED,
    RUMBLE_STARTING,
    RUMBLE_RUNNING,
    RUMBLE_BRAKING
};

class Rumble {
    private:
        uint8_t pin1 = 0, pin2 = 0;
        uint8_t *pin1_ptr, *pin2_ptr;
        float *output_ptr;
        bool enabled = false;
        enum rumble_state state = RUMBLE_STOPPED;
        unsigned long int start_time;
        void setup_pin(uint8_t &old_pin, uint8_t new_pin);

    public:
        void begin(uint8_t &pin1, uint8_t &pin2, float &output_level);
        void update();
        void set_output(bool onoff) { enabled = onoff; }
};