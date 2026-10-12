#pragma once
#include <FastLED.h>
#include "config.h"

struct led_state 
{
    bool on = true;
    uint8_t brightness = 64;
    CRGB color = CRGB::Purple;
    uint8_t white = 0;
    uint8_t effect = 0; // 0 = static, 1 = rainbow, 2 = blink, etc.
    uint8_t speed = 128; // speed of the effect, if applicable
    uint16_t maxCurrent = DEFAULT_MAX_CURRENT_MA; // LED power budget in mA, 0 = hardware max
};

enum Effect : uint8_t {
    Effect_Static = 0,
    Effect_Rainbow = 1,
    Effect_Blink = 2,
    Effect_Breathing = 3,
    Effect_ColorWipe = 4,
    Effect_TheaterChase = 5,
    Effect_Cylon = 6,
    Effect_Sparkle = 7,
    Effect_Fire = 8,
    Effect_Count
};