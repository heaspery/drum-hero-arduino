#ifndef GAME_H
#define GAME_H

#include <Arduino.h>
#include <Chrono.h>

struct Step {
  long time;
  byte lane;
  bool isHit;
  bool isHandled;
};

enum GameState { WAITING,
                 PLAYING,
                 FINISHED };


class Game {
private:
  GameState _state = WAITING;
  int _nSteps;
  Step* _steps;
  Chrono _myChrono;
  int _combo = 0;
  int _lastWinStepIndex = -1;
  int _score = 0;

public:
  const int TOLERANCE = 250;

  void start() {
    _score = 0;
    _combo = 0;
    _lastWinStepIndex = -1;  // on met -1 comme ça on peut considérer le premier index comme réussi
    initializeSteps();       // on génère de nouvelles notes
    _state = PLAYING;
    _myChrono.restart();
  }

  Game(int n)
    : _nSteps(n) {
    _steps = new Step[_nSteps];
  }

  ~Game() {
    delete[] _steps;
  }

  // permet de vérifier si la partie est toujours en cours en comparant le chrono et le timer du dernier step
  void update() {
    if (_state == PLAYING) {
      // 1. On vérifie si des notes ont été manquées pour briser le combo
      checkMissed();

      // 2. On vérifie si on doit finir la partie
      long lastNoteTime = _steps[_nSteps - 1].time;
      if (now() > lastNoteTime + TOLERANCE) {
        _state = FINISHED;
      }
    }
  }

  // on crée des steps avec une base de temps -> chaque step vient après l'autre dans le tableau
  void initializeSteps() {
    randomSeed(analogRead(A5));
    long timeBasis = 1000;
    for (int i = 0; i < _nSteps; i++) {
      timeBasis += random(200, 700);
      _steps[i].time = timeBasis;
      _steps[i].lane = random(3);
      _steps[i].isHit = false;
      _steps[i].isHandled = false;
    }
  }

  void checkInput(int lane, long now) {
    long minDiff = 20000;
    int bestStepIndex = -1;

    for (int i = 0; i < _nSteps; i++) {
        // IMPORTANT: On vérifie si elle n'est pas déjà traitée
        if (_steps[i].lane == lane && !_steps[i].isHandled) {
            long diff = labs(now - _steps[i].time);
            if (diff < minDiff) {
                minDiff = diff;
                bestStepIndex = i;
            }
        }
    }

    if (bestStepIndex != -1) {
        if (minDiff <= TOLERANCE) {
            _score++;
            _steps[bestStepIndex].isHit = true;
            _steps[bestStepIndex].isHandled = true; // Marqué comme fait
            _combo++;
            Serial.println("SUCCESS");
        } else {
            // Optionnel : punir un clic trop précoce
            _combo = 0;
            Serial.println("FAIL_TOO_EARLY");
        }
    }
  } // Fermeture de checkInput

  void checkMissed() {
    long currentTime = now();
    for (int i = 0; i < _nSteps; i++) {
        // Si pas encore traitée et que le temps est dépassé (temps + tolérance)
        if (!_steps[i].isHandled && currentTime > (_steps[i].time + TOLERANCE)) {
            _steps[i].isHandled = true; 
            _steps[i].isHit = false;   
            _combo = 0; 
            Serial.println("MISS: Note passée !");
        }
    }
  }

  // permet de récupérer l'état du jeu
  GameState getState() {
    return _state;
  }

  // permet de mettre à jour l'état du jeu
  void setState(GameState s) {
    _state = s;
  }

  // permet de recommencer le jeu
  void restart() {
    _myChrono.restart();
  }

  // permet de récupérer le temps passé depuis le début du chrono
  long now() {
    return _myChrono.elapsed();
  }

  // permet de récupérer le nombre de steps de la partie
  int getNSteps() {
    return _nSteps;
  }

  // permet de récupérer le timer d'un step grâce à l'index du tableau en paramètre
  long getStepTime(int i) {
    return _steps[i].time;
  }

  // permet de récupérer la couleur de la lane du step grâce à l'index du tableau
  int getStepLane(int i) {
    return _steps[i].lane;
  }

  // permet de récupérer le score
  int getScore() {
    return _score;
  }
};

#endif