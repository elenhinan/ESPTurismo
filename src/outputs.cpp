#include "outputs.h"

void Rumble::begin(uint8_t &pin1, uint8_t &pin2, float &output_level) {
    pin1_ptr = &pin1;
    pin2_ptr = &pin2;
    output_ptr = &output_level;
    setup_pin(this->pin1, *pin1_ptr);
    setup_pin(this->pin2, *pin2_ptr);
    Serial.printf("Rumble on pin %d and %d\n", this->pin1, this->pin2);
}

void Rumble::setup_pin(uint8_t &old_pin, uint8_t new_pin) {
    // do nothing if no change in pins
    if (new_pin == old_pin)
        return;
    // if pin already initialized, set old pin to high impedance
    if (old_pin != 0)
        pinMode(old_pin, INPUT);
    pinMode(new_pin, OUTPUT);
    analogWrite(new_pin, 0);
    old_pin = new_pin;
}

void Rumble::update() {
    // check if pins have changed
    setup_pin(pin1, *pin1_ptr);
    setup_pin(pin2, *pin2_ptr);
    switch(state) {
        case RUMBLE_STOPPED:
            // if going from stopped and output switches to true
            if (enabled) {
                analogWrite(pin1, 255);
                start_time = millis();
                state = RUMBLE_STARTING;
            }
            break;
        case RUMBLE_STARTING:
            // if starting ands output switches to false, set state to stopped
            if (!enabled) {
                analogWrite(pin1, 0);
                state = RUMBLE_STOPPED;
            // if starting and ramp-up time is over, set state to running
            } else if (millis() - start_time > RAMP_UP_TIME) {
                analogWrite(pin1, int(*output_ptr*255));
                state = RUMBLE_RUNNING;
            }
            break;
        case RUMBLE_RUNNING:
            // if running and output switches to false
            if (!enabled) {
                analogWrite(pin1, 0);
                analogWrite(pin2, 255);
                start_time = millis();
                state = RUMBLE_BRAKING;
            }
            break;
        case RUMBLE_BRAKING:
            // if braking and output switches to true
            if (enabled) {
                analogWrite(pin1, int(*output_ptr*255));
                analogWrite(pin2, 0);
                state = RUMBLE_RUNNING;
            // if braking-time is over, set state to stopped
            } else if (millis() - start_time > RAMP_DOWN_TIME) {
                analogWrite(pin2, 0);
                state = RUMBLE_STOPPED;
            }
            break;
    }
}