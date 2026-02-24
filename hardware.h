#ifndef HARDWARE_H
#define HARDWARE_H

#include <Adafruit_NeoPixel.h>

// --- CONFIGURATION PHYSIQUE ---
#define NUMPIXELS 8
#define BRIGHTNESS 10

// Les pins pour les bandes de leds
const int LED_PINS[] = {4, 3, 2}; // Rouge, Vert, Bleu

// Les pins pour les boutons
const int BTN_PINS[] = {13, 12, 11}; // Rouge, Vert, Bleu

// Les pins pour les buzzer Piezo
const int PIEZO_PINS[] = {A0};

// --- OBJETS ---
Adafruit_NeoPixel pixels[] = {
  Adafruit_NeoPixel(NUMPIXELS, LED_PINS[0], NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, LED_PINS[1], NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, LED_PINS[2], NEO_GRB + NEO_KHZ800)
};

// --- INITIALISATION ---
void initHardware() {
  for (int i = 0; i < 3; i++) {
    pinMode(BTN_PINS[i], INPUT_PULLUP);
    pixels[i].begin();
    pixels[i].setBrightness(BRIGHTNESS);
    pixels[i].show(); // Éteint tout au départ
  }

  for(int i = 0; i < 1; i++) {
    pinMode(PIEZO_PINS[i], INPUT);
  }

}

#endif