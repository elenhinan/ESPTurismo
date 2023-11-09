#include "main.h"

//DNSServer dnsServer;

void setup() {
  // setup serial port
  Serial.begin(115200);

  // setup filesystem
  if(!LittleFS.begin(true)){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  // load settings
  settings.load((char*)settings_file);

  // setup ports
  setup_ports();

  // Setup WiFi
  setup_wifi();

  // setup webserver
  webserver.begin();

  // Set up telemetry communication
  gt_telemetry.begin(settings.playstation);
}

void loop() {
  //if (captive) {
  //  dnsServer.processNextRequest();
  //  return;
  //}

  gt_telemetry.update();

  // only turn on rumble if data is recent
  if (gt_telemetry.isValid()) {
    analogWrite(settings.accel_pin, gt_telemetry.getAccel()?settings.accel_output:0);
    analogWrite(settings.decel_pin, gt_telemetry.getDecel()?settings.decel_output:0);
  } else {
    digitalWrite(settings.accel_pin, 0);
    digitalWrite(settings.decel_pin, 0);
  }
  // put your main code here, to run repeatedly:
}

void setup_ports() {
  pinMode(settings.accel_pin, OUTPUT);
  digitalWrite(settings.accel_pin, 0);
  pinMode(settings.decel_pin, OUTPUT);
  digitalWrite(settings.decel_pin, 0);
  pinMode(PIN_LED_WIFI, OUTPUT);
  pinMode(PIN_LED_TELEMETRY, OUTPUT); 
  analogWriteResolution(8);
  analogWriteFrequency(PWM_FREQ);
}

void setup_wifi() {
  Serial.print("Connecting to WiFi");
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(settings.hostname);
  WiFi.begin(settings.wifi_ssid, settings.wifi_pwd);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(PIN_LED_WIFI, !digitalRead(PIN_LED_WIFI));
    delay(500);
    Serial.print(".");
  }
  digitalWrite(PIN_LED_WIFI, HIGH);
  Serial.printf("\nWifi connected to %s, ip: %s\n", WiFi.SSID(),WiFi.localIP().toString());
}