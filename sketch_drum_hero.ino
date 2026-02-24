#include "Hardware.h"
#include "Game.h"

// Variables d'état pour les boutons
int lastBtnState[] = { HIGH, HIGH, HIGH };
unsigned long lastDebounce[] = { 0, 0, 0 };
int value1 = 0;


// On initialise le cerveau du jeu
Game game(30);

void setup() {
  Serial.begin(9600);
  initHardware();  // On appelle la fonction simplifiée du fichier Hardware.h
  Serial.println("Système prêt !");
}

unsigned long lastPiezoTime = 0; 
const int PIEZO_THRESHOLD = 700;
const int PIEZO_COOLDOWN = 100;  

void loop() {
  int value1 = analogRead(A0);
  long currentTime = game.now();

  displayAll(currentTime);
  for (int i = 0; i < 3; i++) {
    checkInput(i, currentTime);
  }

  if (value1 > PIEZO_THRESHOLD && (millis() - lastPiezoTime) > PIEZO_COOLDOWN) {
    
    Serial.print("TAP sur le piezo valeur : ");
    Serial.println(value1);
    lastPiezoTime = millis(); 
  }
}

void displayAll(long now) {
  for (int l = 0; l < 3; l++) pixels[l].clear();

  for (int i = 0; i < game.getNSteps(); i++) {
    long noteTime = game.getStepTime(i);
    int lane = game.getStepLane(i);

    if (noteTime >= now && noteTime <= now + game.TIME_TRAVEL) {
      float progress = (float)(noteTime - now) / game.TIME_TRAVEL;
      int ledPos = progress * (NUMPIXELS - 1);

      uint32_t color = (lane == 0) ? pixels[lane].Color(255, 0, 0) : (lane == 1) ? pixels[lane].Color(0, 255, 0)
                                                                                 : pixels[lane].Color(0, 0, 255);

      pixels[lane].setPixelColor(ledPos, color);
    }
  }
  for (int l = 0; l < 3; l++) pixels[l].show();
}

void checkInput(int lane, long now) {
  int state = digitalRead(BTN_PINS[lane]);
  if (state != lastBtnState[lane] && (millis() - lastDebounce[lane]) > 50) {
    lastDebounce[lane] = millis();
    if (state == LOW) {
      // Logique de détection du hit
      long minDiff = 20000;
      for (int i = 0; i < game.getNSteps(); i++) {
        if (game.getStepLane(i) == lane) {
          minDiff = min(minDiff, labs(now - game.getStepTime(i)));
        }
      }

      if (minDiff <= game.TOLERANCE) Serial.println("HIT!");
      else Serial.println("MISS!");
    }
  }
  lastBtnState[lane] = state;
}