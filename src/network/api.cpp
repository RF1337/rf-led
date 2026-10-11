#include "network/api.h"
#include <WebServer.h>
#include <ArduinoJson.h>
#include "controller/controller.h"
#include "storage/storage.h"
#include "effects/effects.h"

WebServer server(80);

// WiFi setup form, served on GET /. Posts "ssid" and "password" to /wifi.
const char SETUP_PAGE[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>rf-led setup</title>
  <style>
    body { font-family: -apple-system, sans-serif; background: #111; color: #eee;
           max-width: 360px; margin: 40px auto; padding: 0 16px; }
    h1 { font-size: 1.4em; }
    label { display: block; margin-top: 16px; font-size: 0.9em; color: #aaa; }
    input { width: 100%; box-sizing: border-box; padding: 12px; margin-top: 6px;
            font-size: 16px; border: 1px solid #444; border-radius: 8px;
            background: #222; color: #eee; }
    button { width: 100%; margin-top: 24px; padding: 14px; font-size: 16px;
             border: 0; border-radius: 8px; background: #7b2ff7; color: #fff; }
  </style>
</head>
<body>
  <h1>rf-led WiFi setup</h1>
  <form method="POST" action="/wifi">
    <label for="ssid">WiFi name</label>
    <input id="ssid" name="ssid" required autocapitalize="none" autocorrect="off">
    <label for="password">Password</label>
    <input id="password" name="password" type="password">
    <button type="submit">Save &amp; restart</button>
  </form>
</body>
</html>
)rawliteral";

void sendState()
{
  JsonDocument doc;
  doc["on"] = current_led_state.on;
  doc["brightness"] = current_led_state.brightness;
  doc["white"] = current_led_state.white;
  doc["effect"] = current_led_state.effect;
  doc["speed"] = current_led_state.speed;
  JsonArray color = doc["color"].to<JsonArray>();
  color.add(current_led_state.color.r);
  color.add(current_led_state.color.g);
  color.add(current_led_state.color.b);
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleNotFound()
{
  server.sendHeader("Location", "http://" + WiFi.softAPIP().toString() + "/", true);
  server.send(302, "text/plain", "");
}

void handleGetState()
{
  sendState();
}

void handleGetEffects()
{
  JsonDocument doc;
  JsonArray arr = doc.to<JsonArray>();
  for (int i = 0; i < Effect_Count; i++)
  {
    JsonObject o = arr.add<JsonObject>();
    o["id"] = i;
    o["name"] = effectInfo(i).name;
    o["usesColor"] = effectInfo(i).usesColor;
    o["usesSpeed"] = effectInfo(i).usesSpeed;
  }
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleSetState()
{
  String body = server.arg("plain");
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, body);
  if (error)
  {
    server.send(400, "application/json", "{\"error\": \"Invalid JSON\"}");
    return;
  }
  // Process the deserialized JSON data
  if (doc["on"].is<bool>())
  {
    current_led_state.on = doc["on"];
  }
  if (doc["brightness"].is<uint8_t>())
  {
    current_led_state.brightness = doc["brightness"];
  }
  if (doc["color"].is<JsonArray>())
  {
    JsonArray color = doc["color"].as<JsonArray>();
    if (color.size() == 3)
    {
      current_led_state.color.r = color[0];
      current_led_state.color.g = color[1];
      current_led_state.color.b = color[2];
    }
    else
    {
      server.send(400, "application/json", "{\"error\": \"Color array must have 3 elements\"}");
      return;
    }
  }
  if (doc["white"].is<uint8_t>())
  {
    current_led_state.white = doc["white"];
  }

  // Effects
  if (!doc["effect"].isNull())
  { // only validate if "effect" was sent
    int effect = doc["effect"] | -1;
    if (effect < 0 || effect >= Effect_Count)
    {
      server.send(400, "application/json", "{\"error\": \"Invalid effect value\"}");
      return;
    }
    current_led_state.effect = effect;
  }

  // Effect speed
  if (doc["speed"].is<uint8_t>())
  {
    current_led_state.speed = doc["speed"];
  }

  markStateChanged();
  sendState();
}

void handleSetupPage()
{
  server.send(200, "text/html", SETUP_PAGE);
}

void handleSaveWifi()
{
  String ssid = server.arg("ssid");
  String password = server.arg("password");
  if (ssid.isEmpty())
  {
    server.send(400, "text/plain", "WiFi name can't be empty");
    return;
  }
  saveWifiCredentials(ssid, password);
  server.send(200, "text/plain", "Saved! Restarting and connecting to " + ssid + "...");
  delay(1000); // let the response reach the phone before restarting
  ESP.restart();
}

void apiBegin()
{
  server.on("/", HTTP_GET, handleSetupPage);
  server.on("/wifi", HTTP_POST, handleSaveWifi);
  server.on("/api/state", HTTP_GET, handleGetState);
  server.on("/api/state", HTTP_POST, handleSetState);
  server.on("/api/effects", HTTP_GET, handleGetEffects);
  server.onNotFound(handleNotFound);
  server.begin();
}

void apiLoop()
{
  server.handleClient();
}
