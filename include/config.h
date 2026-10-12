#pragma once

#define LED_PIN     21        // GPIO the strip's DIN is connected to
#define NUM_LEDS    30        // how many LEDs on your strip
// Strip is SK6812 RGBW; type + color order are set in led_driver.cpp (NEO_GRBW)
#define BRIGHTNESS  64        // 0-255, keep low while testing (USB power)

// Power limiting (estimates only). Budget is for the LEDs; the ESP itself uses ~100mA on top.
// HARDWARE_MAX is the hard ceiling the API can never exceed: ~80% of the PSU minus the ESP.
// 5V 10A supply -> 10000 * 0.8 - 100 = 7900. Lower it if your wiring/connectors can't take that.
#define HARDWARE_MAX_CURRENT_MA 7900
#define DEFAULT_MAX_CURRENT_MA 400  // safe on USB; 0 = use HARDWARE_MAX
#define LED_MA_PER_CHANNEL     20   // approx. mA for one channel (R/G/B/W) at full on
#define LED_IDLE_MA            1    // approx. mA per LED even when dark

#define ONBOARD_LED 2        // blue LED on the DOIT devkit, used as a heartbeat
