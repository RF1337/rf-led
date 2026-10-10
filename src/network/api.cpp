#include "network/api.h"
#include <WebServer.h>
#include <ArduinoJson.h>
#include "controller/controller.h"

WebServer server(80);

void sendState() {
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

void handleGetState() {
  sendState();
}

void handleSetState() {
    String body = server.arg("plain");
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, body);
    if (error) {
      server.send(400, "application/json", "{\"error\": \"Invalid JSON\"}");
      return;
    }
    // Process the deserialized JSON data
    if (doc.containsKey("on")) {
        current_led_state.on = doc["on"];
    }
    if (doc.containsKey("brightness")) {
        current_led_state.brightness = doc["brightness"];
    }
    if (doc.containsKey("color")) {
        JsonArray color = doc["color"].as<JsonArray>();
        if (color.size() == 3) {
            current_led_state.color.r = color[0];
            current_led_state.color.g = color[1];
            current_led_state.color.b = color[2];
        } else {
            server.send(400, "application/json", "{\"error\": \"Color array must have 3 elements\"}");
            return;
        }
    }
    sendState();
}

void apiBegin() {
  server.on("/api/state", HTTP_GET, handleGetState);
  server.on("/api/state", HTTP_POST, handleSetState);
  server.begin();
}

void apiLoop() {
  server.handleClient();
}
