#include <Arduino.h>
#include <FastLED.h>
#include "config.h"
#include "leds/led_driver.h"
#include "controller/controller.h"
#include <Adafruit_NeoPixel.h>
#include "network/wifi.h"
#include "network/api.h"
#include "storage/storage.h"

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\nrf-led: LED test starting");

  pinMode(ONBOARD_LED, OUTPUT);

  loadLedState(current_led_state);

  ledsBegin();

  wifiBegin();
  apiBegin();
}

void loop() {
  controllerLoop();
  wifiLoop();
  apiLoop();

  if (current_led_state.on) {
    ledsSetBrightness(current_led_state.brightness);
    fill_solid(leds, NUM_LEDS, current_led_state.color);
  } else {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
  }

  ledsShow();

  // Blink the onboard LED and print once a second so you can tell the board is running
  static uint32_t lastBeat = 0;
  if (millis() - lastBeat > 1000) {
    lastBeat = millis();
    digitalWrite(ONBOARD_LED, !digitalRead(ONBOARD_LED));
    Serial.printf("alive\n");
  }

  delay(20);
}