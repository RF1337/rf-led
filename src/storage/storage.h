#pragma once
#include <Arduino.h>
#include "types.h"

void saveWifiCredentials(const String &ssid, const String &password);
bool loadWifiCredentials(String &ssid, String &password);

void saveLedState(const led_state &s);
void loadLedState(led_state &s);