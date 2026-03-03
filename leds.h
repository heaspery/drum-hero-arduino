
#ifndef LEDS_H
#define LEDS_H

#include <Adafruit_NeoPixel.h>
#include "Game.h"

#define NUMPIXELS 72
#define BRIGHTNESS 30

static const int LED_PINS[] = {2, 3, 4};

const int TIME_TRAVEL_BASE = 1200;

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
}

inline void displayLeds(long now, Game &game) {

  for (int l = 0; l < 3; l++) {
    pixels[l].clear();
    pixels[l].setPixelColor(0, pixels[l].Color(10, 10, 10));
  }

  long currentTimeTravel = (long)(TIME_TRAVEL_BASE);
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
    delay(10);
    pixels[l].show();
  }
}

#endif

