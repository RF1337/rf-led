#pragma once

#include <FastLED.h>
#include "config.h"

extern CRGB leds[NUM_LEDS];

void ledsSetBrightness(uint8_t b);
void ledsBegin();
void ledsShow();

void ledsSetWhite(uint8_t w);

void ledsSetMaxCurrent(uint16_t mA);  // clamped to HARDWARE_MAX_CURRENT_MA; 0 = hardware max
uint16_t ledsEstimatedCurrent();      // mA of the last frame, after limiting