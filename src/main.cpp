#include "main.h"

//DNSServer dnsServer;
Rumble rumble_accel;
Rumble rumble_decel;

#ifdef ARDUINO_LOLIN_S2_MINI
  USBCDC SerialUSB;
#endif

void setup() {
  // setup serial port
  DebugSerial.begin(115200);
#ifdef ARDUINO_LOLIN_S2_MINI  
  USB.begin();
  delay(5000);
#endif

  // setup filesystem
  if(!LittleFS.begin(true)){
    DebugSerial.println("An Error has occurred while mounting LittleFS");
  }

  settings.load(settings_file);
  setup_pins();
  rumble_accel.begin(settings.accel_pin1, settings.accel_pin2, settings.accel_output);
  rumble_decel.begin(settings.decel_pin1, settings.decel_pin2, settings.decel_output);
  setup_wifi();
  setup_ota();
  webserver.begin();
  gt_telemetry.begin();
}

void loop() {
  // check for telemetry updates
  if (gt_telemetry.update()) {
    // set rumble output
    rumble_accel.set_output(gt_telemetry.getAccel());
    rumble_decel.set_output(gt_telemetry.getDecel());
    // send event
    char event_buffer[EVENT_BUFFER_SIZE];
    gt_telemetry.to_json(event_buffer, EVENT_BUFFER_SIZE);
    webserver.send_event(event_buffer, "telemetry");
  } else if (!gt_telemetry.isValid()) {
    // if no valid telemetry, turn of rumble
    rumble_accel.set_output(false);
    rumble_decel.set_output(false);
  }
  rumble_accel.update();
  rumble_decel.update();
}

void setup_pins() {
  // setup leds
  pinMode(PIN_LED_WIFI, OUTPUT);
  pinMode(PIN_LED_TELEMETRY, OUTPUT); 
  // setup PWM
  analogWriteResolution(8);
  analogWriteFrequency(PWM_FREQ);
}

void setup_wifi() {
  DebugSerial.print("Connecting to WiFi");
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(settings.hostname);
  // try to connect to wifi
  WiFi.begin(settings.wifi_ssid, settings.wifi_pwd);
  unsigned int wifi_start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(PIN_LED_WIFI, !digitalRead(PIN_LED_WIFI));
    delay(500);
    DebugSerial.print(".");
    // stop trying to connect after timeout
    if (millis() - wifi_start > WIFI_TIMEOUT)
      break;
  }
  // if unable to connect to WiFi, setup as access point so user can configure WiFi
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.mode(WIFI_AP);
    const IPAddress local_ip(10,0,0,1);
    const IPAddress subnet(10,0,0,0);
    WiFi.softAPConfig(local_ip,local_ip,subnet);
    WiFi.softAP(settings.hostname, NULL);
  }
  digitalWrite(PIN_LED_WIFI, HIGH);
  DebugSerial.printf("\nWifi connected to %s, ip: %s\n", WiFi.SSID(),WiFi.localIP().toString());
}

void setup_ota() {
  ArduinoOTA.setHostname(settings.hostname);
  ArduinoOTA.setPassword(OTA_PASSWORD);

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";
        LittleFS.end();

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      DebugSerial.println("Start updating " + type);
    })
    .onEnd([]() {
      DebugSerial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      DebugSerial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      DebugSerial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) DebugSerial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) DebugSerial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) DebugSerial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) DebugSerial.println("Receive Failed");
      else if (error == OTA_END_ERROR) DebugSerial.println("End Failed");
    });

  ArduinoOTA.begin();
  DebugSerial.printf("Ready for OTA @ %s.local, password %s\n",settings.hostname, OTA_PASSWORD);
}