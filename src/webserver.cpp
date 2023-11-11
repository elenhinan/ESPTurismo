#include "webserver.h"

WebServer webserver;

AsyncWebServer server(80);
AsyncEventSource events("/events");

void WebServer::begin() {
    // setup MDNS for easy access
    if (!MDNS.begin(settings.hostname)) {
        Serial.println("Error setting up MDNS responder!");
    } else {
        Serial.println("mDNS responder started");
        MDNS.addService("http", "tcp", 80);
    }
    
    // start webserver
    setup_routes();
    setup_events();
    server.begin();
    Serial.printf("Webserver started at http://%s.local\n", settings.hostname);
}

void WebServer::setup_routes() {
    // static files
    server.serveStatic("/css", LittleFS, "/www/css/");
    server.serveStatic("/js", LittleFS, "/www/js/");

    // index
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){ request->send(LittleFS, "/www/index.html"); });
    
    // settings json getter
    server.on("/rest/settings", HTTP_GET,  [](AsyncWebServerRequest * request) {
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        // create json document and convert to object
        DynamicJsonDocument doc(JSON_MAX_SIZE );
        JsonObject root = doc.to<JsonObject>();
        // get settings to json
        settings.to_json(root);
        serializeJson(root, *response);
        request->send(response);
    });

    // settings json setter
    AsyncCallbackJsonWebHandler *json_setter = new AsyncCallbackJsonWebHandler("/rest/settings", [](AsyncWebServerRequest *request, JsonVariant &json)
       {
           JsonObject root = json.as<JsonObject>();
           settings.from_json(root);
           AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "ok");
           request->send(response);
       }, JSON_MAX_SIZE);
    server.addHandler(json_setter);

    // save settings
    server.on("/rest/save_settings", HTTP_GET,  [](AsyncWebServerRequest * request) {
        AsyncWebServerResponse *response = request->beginResponse(200, "application/json", "{'save':true}");
        request->send(response);
        settings.save(SETTINGS_FILE);
    });

    // remote reset
    server.on("/rest/reboot", HTTP_GET,  [](AsyncWebServerRequest * request) {
        AsyncWebServerResponse *response = request->beginResponse(200, "application/json", "{'reboot':true}");
        request->send(response);
        ESP.restart();
    });
}

void WebServer::setup_events() {
    events.onConnect([](AsyncEventSourceClient *client) {
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    client->send("Hi!", NULL, millis(), 1000);
  });
  server.addHandler(&events);
}

void WebServer::send_event(const char* data, const char* name) {
    events.send(data, name, millis(), 1000);
}