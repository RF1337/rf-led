#include "storage/storage.h"
#include <Preferences.h>
#include "types.h"
#include "FastLED.h"

Preferences preferences;

void saveWifiCredentials(const String &ssid, const String &password) {
  preferences.begin("wifi", false);
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  preferences.end();
}

bool loadWifiCredentials(String &ssid, String &password) {
  preferences.begin("wifi", false);
  ssid = preferences.getString("ssid", "");
  password = preferences.getString("password", "");
  preferences.end();
  return !ssid.isEmpty();
}

void saveLedState(const led_state &s) {
  preferences.begin("led", false);
  preferences.putBool("on", s.on);
  preferences.putUChar("brightness", s.brightness);
  preferences.putUChar("white", s.white);
  preferences.putUChar("r", s.color.r);
  preferences.putUChar("g", s.color.g);
  preferences.putUChar("b", s.color.b);
  preferences.end();
}

void loadLedState(led_state &s) {
  preferences.begin("led", false);
  s.on = preferences.getBool("on", s.on);
  s.brightness = preferences.getUChar("brightness", s.brightness);
  s.white = preferences.getUChar("white", s.white);
  s.color = CRGB(preferences.getUChar("r", s.color.r), preferences.getUChar("g", s.color.g), preferences.getUChar("b", s.color.b));
  preferences.end();
}