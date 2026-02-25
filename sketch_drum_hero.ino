#include "Hardware.h"
#include "Game.h"

// Variables d'état pour les inputs
int lastBtnState[] = { HIGH, HIGH, HIGH };
unsigned long lastDebounce[] = { 0, 0, 0 };
unsigned long lastPiezoTime = 0;
const int PIEZO_THRESHOLD = 700;
const int PIEZO_COOLDOWN = 100;
int value1 = 0;

Game game(100);

void setup() {
  Serial.begin(9600);
  initHardware();
  Serial.println("Système prêt");
}

void loop() {
  // à chaque loop, on vérifie l'état du jeu
  game.update();
  long currentTime = game.now();

  switch (game.getState()) {
    
    case WAITING:
      // On attend le tap pour démarrer
      if (checkStartSignal()) {
        game.start();
        Serial.println("C'est parti !");
      }
      break;

    case PLAYING:
      displayAll(currentTime);
      for (int i = 0; i < 3; i++) checkInput(i, currentTime);
      break;

    case FINISHED:
      Serial.print("Partie terminée ! Score final : ");
      Serial.println(game.getScore());
      //showFinalScore();
      delay(3000); 
      game.setState(WAITING);
      break;
  }
}

// Fonction pour détecter le départ
bool checkStartSignal() {
  int value1 = analogRead(A0);
  return (value1 > PIEZO_THRESHOLD);
}

void displayAll(long now) {
  // clean les différentes lanes à chaque fois
  for (int l = 0; l < 3; l++) pixels[l].clear();

  // parcours l'ensemble du tableau de step, en récupérant son timer et sa lane
  for (int i = 0; i < game.getNSteps(); i++) {
    long noteTime = game.getStepTime(i);
    int lane = game.getStepLane(i);

    // si la note doit encore se jouer et qu'elle doit se jouer avant l'intervale de descente
    if (noteTime >= now && noteTime <= now + game.TIME_TRAVEL) {

      // calcul d'un ratio du temps restant pour ensuite le divisé par le nombre de led - 1 -> donne un chiffre entre 0 et 7
      float progress = (float)(noteTime - now) / game.TIME_TRAVEL;
      int ledPos = progress * (NUMPIXELS - 1);

      // opération ternaire pour définir la couleur du pixel
      uint32_t color = (lane == 0) ? pixels[lane].Color(255, 0, 0) : (lane == 1) ? pixels[lane].Color(0, 255, 0)
                                                                                 : pixels[lane].Color(0, 0, 255);

      pixels[lane].setPixelColor(ledPos, color);
    }
  }
  // affichage des leds
  for (int l = 0; l < 3; l++) pixels[l].show();
}

void checkInput(int lane, long now) {
  // on récupère l'état de chaque bouton
  int state = digitalRead(BTN_PINS[lane]);

  // on regarde l'état du bouton, on utilise un debounce
  if (state != lastBtnState[lane] && (millis() - lastDebounce[lane]) > 50) {
    lastDebounce[lane] = millis();

    if (state == LOW) {

      // mise en place d'une valeur de temps de référence pour faire des comparaisons
      long minDiff = 20000;
      int bestStepIndex = -1;  // permet de mémoriser l'index du step hit

      // on parcourt tous les steps de la game
      for (int i = 0; i < game.getNSteps(); i++) {

        // si le step est dans la lane correspondante
        if (game.getStepLane(i) == lane && !game.getStepIsHit(i)) {

          // on réatribue la différence minimale entre un step et le temps auquel un bouton a été appuyé
          // ainsi que l'index concerné
          long diff = labs(now - game.getStepTime(i));
          if (diff < minDiff) {
            minDiff = diff;
            bestStepIndex = i;
          }
        }
      }

      // si la différence est plus petite ou égale à la tolérance, alors, le step est réussi
      if (bestStepIndex != -1 && minDiff <= game.TOLERANCE) {
        game.setScore(1);

        // on vérifie l'état du step avant celui joué
        if (bestStepIndex == 0 || game.getLastWinStepIndex() == bestStepIndex - 1) {
          game.setCombo(game.getCombo() + 1);  // Augmente le combo
          Serial.print("HIT! Combo: ");
          Serial.println(game.getCombo());
        } else {
          game.setCombo(0);
        }
        game.setStepIsHit(bestStepIndex, true);
        game.setLastWinStepIndex(bestStepIndex);

      } else {
        Serial.println("MISS!");
        game.setCombo(0);  // Le combo se brise sur un mauvais clic
      }
    }

    // état du bouton pour le debounce
    lastBtnState[lane] = state;
  }
}