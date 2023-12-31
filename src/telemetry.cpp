#include "telemetry.h"

GTtelemetry gt_telemetry;

void GTtelemetry::begin() {
    // start listening on port
    Udp.begin(GT_RX_PORT);
    Serial.printf("Listening on UDP port %d\n", GT_RX_PORT);
    last_heartbeat = millis();
}

bool GTtelemetry::update() {
    bool new_data = recieve();
    heartbeat();
    if (new_data)
        digitalWrite(PIN_LED_TELEMETRY, !digitalRead(PIN_LED_TELEMETRY));
    if (!isValid()) {
        digitalWrite(PIN_LED_TELEMETRY, HIGH);
    }
    return new_data;
}

bool GTtelemetry::recieve() {
    uint32_t packetsize = Udp.parsePacket();
    if (packetsize == 0)
        return false;
    data_len = Udp.read(data, GT_BUFFER);
    if (!decrypt()) {
        Serial.println("Packet decode failed");
        return false;
    }
    // good so far, analyze and return true
    last_packet = millis();
    analyze();
    return true;
}

bool GTtelemetry::decrypt() {
    uint32_t nonce[2];
    nonce[1] = get_uint32(0x40);
    nonce[0] = nonce[1] ^ 0xDEADBEAF;
    s20_status_t status = s20_crypt(key, S20_KEYLEN_256, (uint8_t*)nonce , 0, data, data_len);
    return (status == S20_SUCCESS) && (get_uint32(0x00) == 0x47375330);
}

void GTtelemetry::heartbeat() {
    unsigned long now = millis();
    static const unsigned long heartbeat_rate = 1000;
    if (millis() - last_heartbeat >= heartbeat_rate) {
        Udp.beginPacket(settings.playstation, GT_TX_PORT);
        Udp.print('A');
        Udp.endPacket();
        last_heartbeat += heartbeat_rate;
    }
}

void GTtelemetry::analyze() {
    // car speed in m/s
    car_speed = get_float(0x4c) * 3.6; // from m/s to km/h
    slip_decel_any = false;
    slip_accel_any = false;
    wheel_speed_avg = 0;
    for (int i=0;i<4;i++) {
        // wheel rotations (radians/s) times tire radius (m)
        wheel_speed[i] = abs(get_float(0xA4+4*i) * get_float(0xB4+4*i)) * 3.6;
        // calculate wheel grip: 0 = zero grip, 1 = full grip
        float accel_grip = car_speed / wheel_speed[i];
        float decel_grip = wheel_speed[i] / car_speed;
        slip_decel_any |= decel_grip < settings.decel_threshold;
        slip_accel_any |= accel_grip < settings.accel_threshold;
        wheel_slip[i] = min(accel_grip, decel_grip);
        wheel_speed_avg += wheel_speed[i];
    }
    wheel_speed_avg /= 4;
    float accel_grip_avg = car_speed / wheel_speed_avg;
    float decel_grip_avg = wheel_speed_avg / car_speed;
    slip_accel_avg = accel_grip_avg < settings.accel_threshold;
    slip_decel_avg = decel_grip_avg < settings.decel_threshold;
    wheel_slip_avg = min(accel_grip_avg, decel_grip_avg);

    // egen slip for decel og accel, 0-1.. w/c vs c/w

    // pedals
    accel_pedal = float(get_uint8(0x91))/255;
    decel_pedal = float(get_uint8(0x92))/255;

    // flags
    uint16_t flags = get_uint16(0x8e);
    car_active = bitRead(flags,0);
    rev_limit =  bitRead(flags,5);
    asm_active = bitRead(flags,10);
    tcs_active = bitRead(flags,11);
/*
    Serial.printf("Car speed %0.1f, wheel speed %0.1f ", car_speed, wheel_speed_avg);
    Serial.print(slip_decel_avg?"ACCEL ":"      ");
    Serial.print(slip_accel_avg?"DECEL ":"      ");
    Serial.print(asm_active?"ASM ":"    ");
    Serial.print(tcs_active?"TCS ":"    ");
    Serial.print(rev_limit?"REV ":"    ");
    Serial.print("0b");
    for (int aBit = 11; aBit >= 0; aBit--)
        Serial.write(bitRead(flags, aBit) ? '1' : '0');
    Serial.print("\n");
    */
}

bool GTtelemetry::getAccel() {
    // no rumble if car not active, not moving, or pedal not depressed
    if (!car_active || car_speed < settings.accel_min_speed || accel_pedal < settings.accel_min_pedal)
        return false;
    bool active;
    switch(settings.accel_mode) {
        case ANY_WHEEL: active = slip_accel_any; break;
        case AVG_WHEEL: active = slip_accel_avg; break;
        case TCS: active = tcs_active; break;
        case REV_LIMIT: active = rev_limit; break;
        default: active = false;
    }
    return active;
}

bool GTtelemetry::getDecel() {
    // no rumble if car not active, not moving, or pedal not depressed
    if (!car_active || car_speed < settings.decel_min_speed || decel_pedal < settings.decel_min_pedal)
        return false;
    bool active;
    switch(settings.decel_mode) {
        case ANY_WHEEL: active = slip_decel_any; break;
        case AVG_WHEEL: active = slip_decel_avg; break;
        case ASM: active = asm_active; break;
        default: active = false;
    }
    return active;
}

void GTtelemetry::to_json(char* buffer, unsigned int buffer_size) {
    // 250 us to serialize
    if (!isValid()) {
        buffer[0] = 0;
    } else {
        DynamicJsonDocument doc(JSON_MAX_SIZE);
        //doc["car speed"] = car_speed;
        doc["slip FL"] = wheel_slip[0];
        doc["slip FR"] = wheel_slip[1];
        doc["slip RL"] = wheel_slip[2];
        doc["slip RR"] = wheel_slip[3] ;
        doc["slip avg"] = wheel_slip_avg;
        doc["brake"] = decel_pedal;
        doc["throttle"] = accel_pedal;
        doc["active"] = car_active;
        doc["rev limit"] = rev_limit;
        doc["TCS"] = tcs_active;
        doc["ASM"] = asm_active;
        doc["rumble throttle"] = getAccel();
        doc["rumble brake"] = getDecel();
        serializeJson(doc, buffer, buffer_size);
    }
}