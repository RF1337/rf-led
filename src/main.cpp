#include <Arduino.h>
#include <FastLED.h>

// ---- Change these to match your setup ----
#define LED_PIN     21        // GPIO the strip's DIN is connected to
#define NUM_LEDS    30        // how many LEDs on your strip
#define LED_TYPE    WS2812B   // WS2812B, SK6812, WS2811, ...
#define COLOR_ORDER GRB       // if red/green look swapped, try RGB
#define BRIGHTNESS  64        // 0-255, keep low while testing (USB power)

#define ONBOARD_LED 2         // blue LED on the DOIT devkit, used as a heartbeat

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\nrf-led: LED test starting");

  pinMode(ONBOARD_LED, OUTPUT);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);  // stay safe on USB power

  // Startup check: whole strip red, green, blue, half a second each
  const CRGB colors[] = {CRGB::Red, CRGB::Green, CRGB::Blue};
  for (const CRGB &c : colors) {
    fill_solid(leds, NUM_LEDS, c);
    FastLED.show();
    delay(500);
  }
}

void loop() {
  // Scrolling rainbow
  static uint8_t hue = 0;
  fill_rainbow(leds, NUM_LEDS, hue++, 255 / NUM_LEDS);
  FastLED.show();

  // Blink the onboard LED and print once a second so you can tell the board is running
  static uint32_t lastBeat = 0;
  if (millis() - lastBeat > 1000) {
    lastBeat = millis();
    digitalWrite(ONBOARD_LED, !digitalRead(ONBOARD_LED));
    Serial.printf("alive, hue=%u\n", hue);
  }

  delay(20);
}
