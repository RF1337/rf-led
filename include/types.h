#pragma once
#include <FastLED.h>

struct led_state 
{
    bool on = true;
    uint8_t brightness = 64;
    CRGB color = CRGB::Purple;
    uint8_t white = 0;
};
