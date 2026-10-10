#include "network/api.h"
#include <WebServer.h>
#include <ArduinoJson.h>
#include "controller/controller.h"

WebServer server(80);

void handleGetState() {
  JsonDocument doc;
  doc["on"] = current_led_state.on;
  doc["brightness"] = current_led_state.brightness;
  JsonArray color = doc["color"].to<JsonArray>();
  color.add(current_led_state.color.r);
  color.add(current_led_state.color.g);
  color.add(current_led_state.color.b);
  String response; serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void apiBegin() {
  server.on("/api/state", HTTP_GET, handleGetState);
  server.begin();
}

void apiLoop() {
  server.handleClient();
}
