#pragma once

#define LED_PIN     21        // GPIO the strip's DIN is connected to
#define NUM_LEDS    30        // how many LEDs on your strip
// Strip is SK6812 RGBW; type + color order are set in led_driver.cpp (NEO_GRBW)
#define BRIGHTNESS  64        // 0-255, keep low while testing (USB power)

#define ONBOARD_LED 2         // blue LED on the DOIT devkit, used as a heartbeat
