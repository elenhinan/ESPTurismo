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
  settings.load(settings_file);

  // setup ports
  setup_ports();

  // Setup WiFi
  setup_wifi();

  // setup webserver
  webserver.begin();

  // Set up telemetry communication
  gt_telemetry.begin();
}

void loop() {
  // check for telemetry updates
  if (gt_telemetry.update()) {
    // set rumble output
    analogWrite(settings.accel_pin1, gt_telemetry.getAccel()?settings.accel_output:0);
    analogWrite(settings.decel_pin1, gt_telemetry.getDecel()?settings.decel_output:0);
    // send event
    char event_buffer[EVENT_BUFFER_SIZE];
    gt_telemetry.to_json(event_buffer, EVENT_BUFFER_SIZE);
    webserver.send_event(event_buffer, "telemetry");
  } else if (!gt_telemetry.isValid()) {
    // if no valid telemetry, turn of rumble
    digitalWrite(settings.accel_pin1, 0);
    digitalWrite(settings.decel_pin1, 0);
  }
}

void setup_ports() {
  // setupo throttle pins
  pinMode(settings.accel_pin1, OUTPUT);
  digitalWrite(settings.accel_pin1, 0);
  if (settings.accel_pin2 > -1)
  {
    pinMode(settings.accel_pin2, OUTPUT);
    digitalWrite(settings.accel_pin2, 0);
  }
  // setup brake pins
  pinMode(settings.decel_pin1, OUTPUT);
  digitalWrite(settings.decel_pin1, 0);
  if (settings.decel_pin2 > -1)
  {
    pinMode(settings.decel_pin2, OUTPUT);
    digitalWrite(settings.decel_pin2, 0);
  }
  // setup leds
  pinMode(PIN_LED_WIFI, OUTPUT);
  pinMode(PIN_LED_TELEMETRY, OUTPUT); 
  // setup PWM
  analogWriteResolution(8);
  analogWriteFrequency(PWM_FREQ);
}

void setup_wifi() {
  Serial.print("Connecting to WiFi");
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(settings.hostname);
  // try to connect to wifi
  WiFi.begin(settings.wifi_ssid, settings.wifi_pwd);
  unsigned int wifi_start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(PIN_LED_WIFI, !digitalRead(PIN_LED_WIFI));
    delay(500);
    Serial.print(".");
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
  Serial.printf("\nWifi connected to %s, ip: %s\n", WiFi.SSID(),WiFi.localIP().toString());
}