#pragma once
#include <Arduino.h>

void saveWifiCredentials(const String &ssid, const String &password);
bool loadWifiCredentials(String &ssid, String &password);