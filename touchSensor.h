#ifndef TOUCHSENSOR_H
#define TOUCHSENSOR_H

#include <Arduino.h>

static const uint8_t SENSOR_PINS[] = { 10, 11, 12 };
// cooldown
static const unsigned long HIT_COOLDOWN_MS = 250;

static unsigned long lastTouchHitTime[3] = { 0, 0, 0 };
static uint8_t lastStableState[3] = { HIGH, HIGH, HIGH };
static uint8_t lastReadState[3] = { HIGH, HIGH, HIGH };
static unsigned long lastChangeTime[3] = { 0, 0, 0 };

static const unsigned long DEBOUNCE_MS = 25;
static const uint8_t ACTIVE_STATE = LOW;

inline void initTouchSensors() {
  for (int i = 0; i < 3; i++) {
    pinMode(SENSOR_PINS[i], INPUT_PULLUP);
  }
}

// Retourne true une seule fois par touché + debounce + cooldown
inline bool isTouchHit(int lane) {
  const unsigned long now = millis();
  const uint8_t pin = SENSOR_PINS[lane];

  uint8_t reading = digitalRead(pin);

  // Détection de changement brut
  if (reading != lastReadState[lane]) {
    lastReadState[lane] = reading;
    lastChangeTime[lane] = now;
  }

  // Si l'état est stable assez longtemps, on l'accepte comme stable
  if ((now - lastChangeTime[lane]) >= DEBOUNCE_MS) {
    if (lastStableState[lane] != reading) {
      lastStableState[lane] = reading;

      // On déclenche sur l'entrée dans l'état actif
      if (reading == ACTIVE_STATE) {
        if (now - lastTouchHitTime[lane] > HIT_COOLDOWN_MS) {
          lastTouchHitTime[lane] = now;
          return true;
        }
      }
    }
  }

  return false;
}

#endif