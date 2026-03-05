#ifndef LEDS_H
#define LEDS_H

#include <Adafruit_NeoPixel.h>
#include "Game.h"

#define NUMPIXELS 72
#define BRIGHTNESS 30

static const int LED_PINS[] = {2, 3, 4};

// time (ms) for a step to travel along the strip (lower = faster)
const int TIME_TRAVEL_BASE = 1500; // CHANGE ME IF YOU WANT TO ADAPT THE GAME DIFFICULTY

inline Adafruit_NeoPixel pixels[] = {
  Adafruit_NeoPixel(NUMPIXELS, LED_PINS[0], NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, LED_PINS[1], NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, LED_PINS[2], NEO_GRB + NEO_KHZ800)
};

// initialize led strips
inline void initHardware() {
  for (int i = 0; i < 3; i++) {
    pixels[i].begin();
    pixels[i].setBrightness(BRIGHTNESS);
    pixels[i].show();
  }
}

inline void displayLeds(long now, Game &game) {

  // clear all leds
  for (int l = 0; l < 3; l++) {
    pixels[l].clear();
  }

  
  long currentTimeTravel = (long)(TIME_TRAVEL_BASE);
  if (currentTimeTravel < 1) currentTimeTravel = 1; 

  // for each step, check if it should appear on the strip
  for (int i = 0; i < game.getNSteps(); i++) {
    long noteTime = game.getStepTime(i);

    // skip steps already passed
    // stop when steps are too far in the future
    if (noteTime < now) continue;
    if (noteTime > now + currentTimeTravel) break;

    int lane = game.getStepLane(i);

    // compute led position based on time progress
    float progress = (float)(noteTime - now) / (float)currentTimeTravel;
    int ledPos = (int)(progress * (NUMPIXELS - 1)); // map progress to strip position

    // color depending on lane
    uint32_t cHead;
    if (lane == 0)      cHead = pixels[lane].Color(255, 0, 0);
    else if (lane == 1) cHead = pixels[lane].Color(0, 255, 0);
    else                cHead = pixels[lane].Color(0, 0, 255);

    // draw the step if inside the strip
    if (ledPos >= 0 && ledPos < NUMPIXELS) {
      pixels[lane].setPixelColor(ledPos, cHead);
      if (ledPos + 1 < NUMPIXELS) pixels[lane].setPixelColor(ledPos + 1, cHead / 2);
    }
  }

  // update the led strips
  for (int l = 0; l < 3; l++) {
    delay(10);
    pixels[l].show();
  }
}

#endif