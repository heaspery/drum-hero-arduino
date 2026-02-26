#ifndef PIEZO_H
#define PIEZO_H

#include <Arduino.h>

static const int PIEZO_PINS[] = {A2, A1, A0};
static const int THRESHOLD = 400;     

static unsigned long lastHitTime[3] = {0, 0, 0};

inline bool isPiezoHit(int lane) {
    int val = analogRead(PIEZO_PINS[lane]);
    unsigned long now = millis();

    if (val > THRESHOLD && (now - lastHitTime[lane] > 250)) {
        lastHitTime[lane] = now;
        return true;
    }
    return false;
}

#endif