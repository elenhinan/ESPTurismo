#pragma once
#include <Arduino.h>
#include <salsa20.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include "config.h"
#include "settings.h"

#define GT_TX_PORT 33739
#define GT_RX_PORT 33740
#define GT_KEY "Simulator Interface Packet GT7 ver 0.0"
#define GT_BUFFER 512

class GTtelemetry {
    private:
        // communication stuff
        WiFiUDP Udp;
        s20_status_t status;
        uint8_t* key = (uint8_t*)GT_KEY;
        uint8_t data[GT_BUFFER];
        uint32_t data_len;
        unsigned long last_heartbeat;
        unsigned long last_packet = 0;

        // telemetry values
        float car_speed;
        float wheel_speed[4];
        float wheel_speed_avg;
        float wheel_slip[4];
        float wheel_slip_avg;
        float decel_pedal;
        float accel_pedal;
        bool car_active;
        bool rev_limit;
        bool tcs_active;
        bool asm_active;

        // output logic
        bool slip_accel_any; // slipping while trying to accelerate/throttle
        bool slip_decel_any; // slipping while trying to decelerate/brake
        bool slip_accel_avg; // slipping while trying to accelerate/throttle
        bool slip_decel_avg; // slipping while trying to decelerate/brake

        // extract data from package
        float get_float(unsigned int adr)   { return *((float *)&data[adr]); };
        uint8_t get_uint8(unsigned int adr) { return data[adr]; };
        uint8_t get_uint16(unsigned int adr) { return *((uint16_t *)&data[adr]); };
        uint32_t get_uint32(unsigned int adr) { return *((uint32_t *)&data[adr]); };

        bool recieve();
        bool decrypt();
        void heartbeat();
        void analyze();
        void send_data();
    
    public:
        void begin();
        bool update();
        bool getAccel();
        bool getDecel();
        bool isValid() { return millis() - last_packet < 100; }
        void to_json(char* buffer, unsigned int buffer_size);
};

extern GTtelemetry gt_telemetry;