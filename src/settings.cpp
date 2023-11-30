#include "settings.h"

Settings settings;

void Settings::load(const char* filename) {
    // parse settings file from filesystem
    File file = LittleFS.open(filename);
    DynamicJsonDocument doc(JSON_MAX_SIZE);
    DeserializationError error = deserializeJson(doc, file);

    if (error)
        Serial.println(F("Failed to read file, using default configuration"));
    
    JsonObject root = doc.as<JsonObject>();
    from_json(root);
    file.close();
}

void Settings::save(const char* filename) {
    File file = LittleFS.open(filename, "w+");
    DynamicJsonDocument doc(JSON_MAX_SIZE);
    JsonObject root = doc.to<JsonObject>();
    settings.to_json(root);
    serializeJson(root, file);
    file.close();
}

void Settings::from_json(JsonObject &root) {
    strlcpy(wifi_ssid, root["network"]["wifi_ssid"] | "", sizeof(wifi_ssid));
    strlcpy(wifi_pwd, root["network"]["wifi_pwd"] | "", sizeof(wifi_pwd));
    strlcpy(hostname, root["network"]["hostname"] | "", sizeof(hostname));
    strlcpy(playstation, root["network"]["playstation"] | "", sizeof(playstation));

    decel_pin1          = root["decel"]["pin1"]      | PIN_DECEL1;
    decel_pin2          = root["decel"]["pin2"]      | PIN_DECEL2;
    decel_output        = root["decel"]["output"]    | 0.8f;
    decel_threshold     = root["decel"]["threshold"] | 0.8f;
    decel_mode          = root["decel"]["mode"]      | ANY_WHEEL;
    decel_min_pedal     = root["decel"]["min_pedal"] | 0.02f;
    decel_min_speed     = root["decel"]["min_vel"]   | 10.0f;

    accel_pin1          = root["accel"]["pin1"]      | PIN_ACCEL1;
    accel_pin2          = root["accel"]["pin2"]      | PIN_ACCEL2;
    accel_output        = root["accel"]["output"]    | 0.4f;
    accel_threshold     = root["accel"]["threshold"] | 0.8f;
    accel_mode          = root["accel"]["mode"]      | ANY_WHEEL;
    accel_min_pedal     = root["accel"]["min_pedal"] | 0.02f;
    accel_min_speed     = root["accel"]["min_vel"]   | 1.0f;

    Serial.println("Loading config:");
    serializeJsonPretty(root, Serial);
    Serial.println("");
}

void Settings::to_json(JsonObject &root) {
    root["network"]["wifi_ssid"]    = wifi_ssid;
    root["network"]["wifi_pwd"]     = wifi_pwd;
    root["network"]["hostname"]     = hostname;
    root["network"]["playstation"]  = playstation;

    root["decel"]["pin1"]        = decel_pin1;
    root["decel"]["pin2"]        = decel_pin2;
    root["decel"]["output"]      = decel_output;
    root["decel"]["threshold"]   = decel_threshold;
    root["decel"]["mode"]        = decel_mode;
    root["decel"]["min_pedal"]   = decel_min_pedal;
    root["decel"]["min_vel"]     = decel_min_speed;

    root["accel"]["pin1"]        = accel_pin1;
    root["accel"]["pin2"]        = accel_pin2;
    root["accel"]["output"]      = accel_output;
    root["accel"]["threshold"]   = accel_threshold;
    root["accel"]["mode"]        = accel_mode;
    root["accel"]["min_pedal"]   = accel_min_pedal;
    root["accel"]["min_vel"]     = accel_min_speed;
}