#include "Leds.h"
#include "Game.h"
#include "Piezo.h"

// Variables d'état pour les inputs
int value1 = 0;

Game game(20);

void setup() {
  Serial.begin(9600);
  initHardware();
  Serial.println("Système prêt");
}

void loop() {
  game.update();  // Met à jour le timer interne et vérifie si la game est finie
  long currentTime = game.now(); // récupère le chrono de la partie

  switch (game.getState()) {

    case WAITING:
      // On utilise le piezo de la lane 0 pour démarrer
      if (isPiezoHit(0)) {
        game.start();
        Serial.println("Let's go!");
        delay(200);  // Petit délai pour ne pas trigger une note direct au départ
      }
      break;

    case PLAYING:
      displayLeds(currentTime, game);

      // On surveille les capteurs en continu
      for (int i = 0; i < 3; i++) {
        if (isPiezoHit(i)) {
          game.checkInput(i, currentTime);
        }
      }
      break;

    case FINISHED:
      Serial.print("Termine ! Score : ");
      Serial.println(game.getScore());
      delay(3000);
      game.setState(WAITING);
      break;
  }
}
