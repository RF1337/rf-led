#include "effects/effects.h"
#include "leds/led_driver.h"
#include "config.h"

// All effects are time-based (millis()), so they run at the same speed no
// matter how fast loop() is. s.speed (0-255) maps to slow..fast per effect.

// Maps speed 0..255 onto a range, e.g. a period from slowMs down to fastMs.
static uint32_t speedMap(uint8_t speed, uint32_t slow, uint32_t fast) {
    return map(speed, 0, 255, slow, fast);
}

static void solid(const led_state &s) {
    fill_solid(leds, NUM_LEDS, s.color);
}

static void rainbow(const led_state &s) {
    // speed 0 = one cycle in ~4 minutes, 128 = ~2s, 255 = ~1s
    uint8_t hue = (millis() * (s.speed + 1)) >> 10;
    fill_rainbow(leds, NUM_LEDS, hue, max(1, 255 / NUM_LEDS));
}

static void blink(const led_state &s) {
    uint32_t period = speedMap(s.speed, 2000, 100);
    bool on = (millis() % period) < period / 2;
    fill_solid(leds, NUM_LEDS, on ? s.color : CRGB::Black);
}

static void breathing(const led_state &s) {
    uint8_t bpm = speedMap(s.speed, 4, 60);
    uint8_t level = beatsin8(bpm, 10, 255);  // smooth sine between 10 and 255
    CRGB c = s.color;
    c.nscale8_video(level);
    fill_solid(leds, NUM_LEDS, c);
}

static void colorWipe(const led_state &s) {
    // Fill the strip one LED at a time, then clear it one LED at a time
    uint32_t stepMs = speedMap(s.speed, 200, 10);
    uint32_t step = (millis() / stepMs) % (2 * NUM_LEDS);
    for (int i = 0; i < NUM_LEDS; i++) {
        bool lit = step < NUM_LEDS ? i <= (int)step : i > (int)(step - NUM_LEDS);
        leds[i] = lit ? s.color : CRGB::Black;
    }
}

static void theaterChase(const led_state &s) {
    // Every third LED lit, marching along the strip
    uint32_t stepMs = speedMap(s.speed, 400, 30);
    uint8_t offset = (millis() / stepMs) % 3;
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = (i % 3 == offset) ? s.color : CRGB::Black;
    }
}

static void cylon(const led_state &s) {
    // A dot bouncing back and forth with a short fading tail
    if (NUM_LEDS < 2) return solid(s);
    uint32_t stepMs = speedMap(s.speed, 120, 8);
    uint32_t step = (millis() / stepMs) % (2 * (NUM_LEDS - 1));
    int pos = step < NUM_LEDS ? step : 2 * (NUM_LEDS - 1) - step;
    const uint8_t tail[] = {255, 90, 25};  // brightness by distance from the dot
    for (int i = 0; i < NUM_LEDS; i++) {
        int dist = abs(i - pos);
        CRGB c = s.color;
        c.nscale8_video(dist < 3 ? tail[dist] : 0);
        leds[i] = c;
    }
}

static void sparkle(const led_state &s) {
    // Random LEDs flash in the chosen color and fade out.
    // Needs the previous frame, so it only updates every 20ms.
    static uint32_t last = 0;
    if (millis() - last < 20) return;
    last = millis();

    fadeToBlackBy(leds, NUM_LEDS, 30);
    uint8_t chance = speedMap(s.speed, 5, 180);  // odds of a new sparkle each frame
    if (random8() < chance) {
        leds[random16(NUM_LEDS)] = s.color;
    }
}

static void fire(const led_state &s) {
    // Classic "Fire2012" by Mark Kriegsman: heat rises from the start of the strip
    static uint8_t heat[NUM_LEDS];
    static uint32_t last = 0;
    uint32_t frameMs = speedMap(s.speed, 60, 10);
    if (millis() - last < frameMs) return;
    last = millis();

    const uint8_t cooling = 55;    // higher = shorter flames
    const uint8_t sparking = 120;  // higher = more roaring fire

    // 1. Every cell cools down a little
    for (int i = 0; i < NUM_LEDS; i++) {
        heat[i] = qsub8(heat[i], random8(0, (cooling * 10) / NUM_LEDS + 2));
    }
    // 2. Heat drifts up and diffuses
    for (int k = NUM_LEDS - 1; k >= 2; k--) {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }
    // 3. Randomly ignite new sparks near the bottom
    if (random8() < sparking) {
        int y = random8(min(7, NUM_LEDS));
        heat[y] = qadd8(heat[y], random8(160, 255));
    }
    // 4. Turn heat into colors (black -> red -> yellow -> white)
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = HeatColor(heat[i]);
    }
}

void effectsRender(const led_state &s) {
    switch (s.effect) {
        case Effect_Static:       solid(s);        break;
        case Effect_Rainbow:      rainbow(s);      break;
        case Effect_Blink:        blink(s);        break;
        case Effect_Breathing:    breathing(s);    break;
        case Effect_ColorWipe:    colorWipe(s);    break;
        case Effect_TheaterChase: theaterChase(s); break;
        case Effect_Cylon:        cylon(s);        break;
        case Effect_Sparkle:      sparkle(s);      break;
        case Effect_Fire:         fire(s);         break;
        default:                  solid(s);        break;
    }
}
