#pragma once
#include "types.h"

struct EffectInfo {
    const char *name;
    bool usesColor;
    bool usesSpeed;
};

const EffectInfo &effectInfo(uint8_t id);

void effectsRender(const led_state &s);
