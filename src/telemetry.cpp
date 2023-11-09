#include "telemetry.h"

GTtelemetry gt_telemetry;

void GTtelemetry::begin(const char* host_ip) {
    this->host_ip = host_ip;
    // start listening on port
    Udp.begin(GT_RX_PORT);
    Serial.printf("Listening on UDP port %d\n", GT_RX_PORT);
    last_heartbeat = millis();
}

void GTtelemetry::update() {
    recieve();
    heartbeat();
    if (!isValid()) {
        digitalWrite(PIN_LED_TELEMETRY, HIGH);
    }
}

void GTtelemetry::recieve() {
    uint32_t packetsize = Udp.parsePacket();
    if (packetsize == 0)
        return;
    data_len = Udp.read(data, GT_BUFFER);
    if (!decode()) {
        Serial.println("Packet decode failed");
        return;
    }
    last_packet = millis();
    digitalWrite(PIN_LED_TELEMETRY, !digitalRead(PIN_LED_TELEMETRY));
    analyze();
}

bool GTtelemetry::decode() {
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
        Udp.beginPacket(host_ip, GT_TX_PORT);
        Udp.print('A');
        Udp.endPacket();
        //Serial.printf("\nHeartbeat sent to %s:%d\n",host_ip,GT_TX_PORT);
        last_heartbeat += heartbeat_rate;
    }
}

void GTtelemetry::analyze() {
    // car speed in m/s
    car_speed = get_float(0x4c);
    slip_decel_any = false;
    slip_accel_any = false;
    wheel_speed_avg = 0;
    float wheel_slip_ratio;
    for (int i=0;i<4;i++) {
        // wheel rotations (radians/s) times tire radius (m)
        wheel_speed[i] = abs(get_float(0xA4+4*i) * get_float(0xB4+4*i));
        wheel_slip_ratio = wheel_speed[i] / car_speed;
        slip_decel_any |= wheel_slip_ratio < settings.decel_threshold;
        slip_accel_any |= wheel_slip_ratio > settings.accel_threshold;
        wheel_speed_avg += wheel_speed[i];
    }
    wheel_speed_avg /= 4;
    wheel_slip_ratio = wheel_speed_avg / car_speed;
    slip_decel_avg = wheel_slip_ratio < settings.decel_threshold;
    slip_accel_avg = wheel_slip_ratio > settings.accel_threshold;

    // pedals
    accel_pedal = get_uint8(0x91);
    decel_pedal = get_uint8(0x92);

    // flags
    uint16_t flags = get_uint16(0x8e);
    car_active = bitRead(flags,0);
    rev_limit =  bitRead(flags,5);
    asm_active = bitRead(flags,10);
    tcs_active = bitRead(flags,11);

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

}

bool GTtelemetry::getAccel() {
    // no rumble if car not active, not moving, or pedal not depressed
    if (!car_active || car_speed < settings.accel_min_speed || accel_pedal < settings.accel_min_pedal)
        return false;
    bool active;
    switch(settings.accel_mode) {
        case ANY_WHEEL: active = slip_accel_any;
        case AVG_WHEEL: active = slip_accel_avg;
        case TCS: active = tcs_active;
        case REV_LIMIT: active = rev_limit;
        default: active = false;
    }
    return active;
}

bool GTtelemetry::getDecel() {
    // no rumble if car not active, not moving, or pedal not depressed
    if (!car_active || car_speed < settings.decel_min_speed || accel_pedal < settings.decel_min_pedal)
        return false;
    bool active;
    switch(settings.decel_mode) {
        case ANY_WHEEL: active = slip_decel_any;
        case AVG_WHEEL: active = slip_decel_avg;
        case ASM: active = asm_active;
        default: active = false;
    }
    return active;
}

void GTtelemetry::get_json(JsonObject &root) {
    if (!isValid())
        return;
    root["speed"] = car_speed;
    root["wheel FL"] = wheel_speed[0];
    root["wheel FR"] = wheel_speed[1];
    root["wheel RL"] = wheel_speed[2];
    root["wheel RR"] = wheel_speed[3];
    root["wheel avg"] = wheel_speed_avg;
    root["brake"] = decel_pedal;
    root["throttle"] = accel_pedal;
    root["active"] = car_active;
    root["rev limit"] = rev_limit;
    root["TCS"] = tcs_active;
    root["ASM"] = asm_active;
}