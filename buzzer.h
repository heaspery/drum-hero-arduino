#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>
#include "pitches.h"

static const int BUZZER_PIN = 6;

inline void playNote(int frequency, int duration = 400) {
  if (frequency > 0) {
    tone(BUZZER_PIN, frequency, duration);
  }
}

#endif