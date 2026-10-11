#include "controller.h"
#include "storage/storage.h"

led_state current_led_state;

bool stateDirty = false;
unsigned long lastStateChangeTime = 0;

void markStateChanged() {
    stateDirty = true;
    lastStateChangeTime = millis();
}

void controllerLoop() {
    if (stateDirty && (millis() - lastStateChangeTime > 2000)) {
        saveLedState(current_led_state);
        stateDirty = false;
        Serial.printf("Saved LED state: on=%d, brightness=%d, color=(%d,%d,%d)\n", current_led_state.on, current_led_state.brightness, current_led_state.color.r, current_led_state.color.g, current_led_state.color.b);
    }
}