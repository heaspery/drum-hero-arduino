#ifndef BUZZER_H
#define BUZZER_H

#include <TimerFreeTone.h>
#include <Arduino.h>
#include "pitches.h"
#include "game.h"

static const int BUZZER_PIN = 6;

int melody[] = { 262, 196, 196, 220, 196, 247, 262 };
int index = 0;

inline void playNote() {   
  tone(BUZZER_PIN, melody[index], 80); 
  index++;
  if(index == 7) {
    index = 0;
  }
}

#endif