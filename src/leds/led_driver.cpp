#include <FastLED.h>
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include "led_driver.h"
#include "config.h"

CRGB leds[NUM_LEDS];

// FastLED is only used for color math on leds[]; NeoPixel sends it to the RGBW strip
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRBW + NEO_KHZ800);

void ledsSetBrightness(uint8_t b) {
  strip.setBrightness(b);
}

void ledsBegin() {
  strip.begin();

  // Startup check: whole strip red, green, blue, half a second each
  const CRGB colors[] = {CRGB::Red, CRGB::Green, CRGB::Blue};
  for (const CRGB &c : colors) {
    fill_solid(leds, NUM_LEDS, c);
    ledsShow();
    delay(1000);
  }
}

void ledsShow() {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, leds[i].r, leds[i].g, leds[i].b, 0);  // white channel off for now
  }
  strip.show();
}