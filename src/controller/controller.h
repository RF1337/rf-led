#pragma once
#include "types.h"

extern led_state current_led_state;
void markStateChanged();
void controllerLoop();