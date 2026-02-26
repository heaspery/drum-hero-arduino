#ifndef LEDS_H
#define LEDS_H

#include <Adafruit_NeoPixel.h>
#include "Game.h"

// constantes des leds
#define NUMPIXELS 72
#define BRIGHTNESS 50

static const int LED_PINS[] = { 4, 3, 2 };
const int TIME_TRAVEL = 2000;


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

// fonction qui permet d'afficher les leds au bon moment
inline void displayLeds(long now, Game &game) {
  for (int l = 0; l < 3; l++) pixels[l].clear();

  for (int i = 0; i < game.getNSteps(); i++) {
    long noteTime = game.getStepTime(i);
    int lane = game.getStepLane(i);

    if (noteTime >= now && noteTime <= now + TIME_TRAVEL) {
      float progress = (float)(noteTime - now) / TIME_TRAVEL;

      // Inversion pour que la note défile vers la première LED
      int ledPos = progress * (NUMPIXELS - 1);

      uint32_t color = (lane == 0) ? pixels[lane].Color(255, 0, 0) : (lane == 1) ? pixels[lane].Color(0, 255, 0)
                                                                                 : pixels[lane].Color(0, 0, 255);

      // On définit des variantes de la couleur pour l'effet de traînée
      // On diminue les composantes RGB pour réduire l'éclat
      uint32_t colorHead = color;
      uint32_t colorMid = (lane == 0) ? pixels[lane].Color(100, 0, 0) : (lane == 1) ? pixels[lane].Color(0, 100, 0)
                                                                                    : pixels[lane].Color(0, 0, 100);
      uint32_t colorTail = (lane == 0) ? pixels[lane].Color(30, 0, 0) : (lane == 1) ? pixels[lane].Color(0, 30, 0)
                                                                                    : pixels[lane].Color(0, 0, 30);

      // Affichage des 3 pixels avec sécurité pour ne pas sortir du ruban (0 à 71)
      if (ledPos >= 0 && ledPos < NUMPIXELS) {
        pixels[lane].setPixelColor(ledPos, colorHead);
      }
      if (ledPos + 1 >= 0 && ledPos + 1 < NUMPIXELS) {
        pixels[lane].setPixelColor(ledPos + 1, colorMid);
      }
      if (ledPos + 2 >= 0 && ledPos + 2 < NUMPIXELS) {
        pixels[lane].setPixelColor(ledPos + 2, colorTail);
      }
      if (ledPos + 2 >= 0 && ledPos + 3 < NUMPIXELS) {
        pixels[lane].setPixelColor(ledPos + 3, colorTail);
      }
      if (ledPos + 2 >= 0 && ledPos + 2 < NUMPIXELS) {
        pixels[lane].setPixelColor(ledPos + 3, colorTail);
      }
    }
  }
  for (int l = 0; l < 3; l++) pixels[l].show();
}

#endif