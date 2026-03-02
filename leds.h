#ifndef LEDS_H
#define LEDS_H

#include <Adafruit_NeoPixel.h>
#include "Game.h"

#define NUMPIXELS 72
#define BRIGHTNESS 10

static const int LED_PINS[] = {2, 3, 4};

const int TIME_TRAVEL_BASE = 4000;

// gestion de la difficulté
static float currentDifficulty = 1.0f;
static float targetDifficulty  = 1.0f;   
static unsigned long lastDifficultyUpdate = 0;
static unsigned long flashUntilMs = 0; 
static unsigned long lastFrameMs = 0;

inline Adafruit_NeoPixel pixels[] = {
  Adafruit_NeoPixel(NUMPIXELS, LED_PINS[0], NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, LED_PINS[1], NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, LED_PINS[2], NEO_GRB + NEO_KHZ800)
};

inline void initHardware() {
  for (int i = 0; i < 3; i++) {
    pixels[i].begin();
    pixels[i].setBrightness(BRIGHTNESS);
    pixels[i].show();
  }
  lastDifficultyUpdate = millis(); 
}

inline void displayLeds(long now, Game &game) {

  unsigned long ms = millis();

  if (ms - lastDifficultyUpdate > 5000) {
    if (currentDifficulty > 0.4f) {
      currentDifficulty -= 0.20f;
      flashUntilMs = ms + 120; 
    }
    lastDifficultyUpdate = ms;
  }

  for (int l = 0; l < 3; l++) {
    pixels[l].clear();
    pixels[l].setPixelColor(0, pixels[l].Color(10, 10, 10));
  }

  long currentTimeTravel = (long)(TIME_TRAVEL_BASE * currentDifficulty);
  if (currentTimeTravel < 1) currentTimeTravel = 1; 

  for (int i = 0; i < game.getNSteps(); i++) {
    long noteTime = game.getStepTime(i);

    if (noteTime < now) continue;
    if (noteTime > now + currentTimeTravel) break;

    int lane = game.getStepLane(i);

    float progress = (float)(noteTime - now) / (float)currentTimeTravel;
    int ledPos = (int)(progress * (NUMPIXELS - 1));

    uint32_t cHead;
    if (lane == 0)      cHead = pixels[lane].Color(255, 0, 0);
    else if (lane == 1) cHead = pixels[lane].Color(0, 255, 0);
    else                cHead = pixels[lane].Color(0, 0, 255);

    if (ledPos >= 0 && ledPos < NUMPIXELS) {
      pixels[lane].setPixelColor(ledPos, cHead);
      if (ledPos + 1 < NUMPIXELS) pixels[lane].setPixelColor(ledPos + 1, cHead / 2);
    }
  }

  for (int l = 0; l < 3; l++) {
    pixels[l].show();
  }
}

#endif

