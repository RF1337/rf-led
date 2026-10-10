#pragma once

#include <FastLED.h>
#include "config.h"

extern CRGB leds[NUM_LEDS];

void ledsSetBrightness(uint8_t b);
void ledsBegin();
void ledsShow();