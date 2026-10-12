#include <FastLED.h>
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include "led_driver.h"
#include "config.h"

CRGB leds[NUM_LEDS];

// FastLED is only used for color math on leds[]; NeoPixel sends it to the RGBW strip
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRBW + NEO_KHZ800);

static uint8_t whiteLevel = 0;
static uint8_t requestedBrightness = BRIGHTNESS;
static uint16_t maxCurrentMa = DEFAULT_MAX_CURRENT_MA;
static uint16_t estimatedCurrentMa = 0;

void ledsSetWhite(uint8_t w) {
  whiteLevel = w;
}

// Brightness is applied in ledsShow(), after the power limiter has had its say
void ledsSetBrightness(uint8_t b) {
  requestedBrightness = b;
}

// Never above the hardware ceiling; 0 means "as much as the hardware allows"
void ledsSetMaxCurrent(uint16_t mA) {
  maxCurrentMa = (mA == 0 || mA > HARDWARE_MAX_CURRENT_MA) ? HARDWARE_MAX_CURRENT_MA : mA;
}

uint16_t ledsEstimatedCurrent() {
  return estimatedCurrentMa;
}

// Estimates the strip's current draw and lowers brightness if it's over budget
static uint8_t limitBrightness() {
  uint32_t channelSum = 0;  // all channels of all LEDs, 0-255 each
  for (int i = 0; i < NUM_LEDS; i++) {
    channelSum += leds[i].r + leds[i].g + leds[i].b + whiteLevel;
  }
  const uint32_t idleMa = NUM_LEDS * LED_IDLE_MA;
  uint32_t litMa = channelSum * LED_MA_PER_CHANNEL / 255 * requestedBrightness / 255;

  uint8_t brightness = requestedBrightness;
  if (litMa + idleMa > maxCurrentMa) {
    uint32_t budget = maxCurrentMa > idleMa ? maxCurrentMa - idleMa : 0;
    brightness = (uint32_t)requestedBrightness * budget / litMa;
    litMa = litMa * brightness / requestedBrightness;
  }
  estimatedCurrentMa = litMa + idleMa;
  return brightness;
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
  strip.setBrightness(limitBrightness());
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, leds[i].r, leds[i].g, leds[i].b, whiteLevel);
  }
  strip.show();
}