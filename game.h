#ifndef GAME_H
#define GAME_H

#include <Arduino.h>
#include <Chrono.h>

struct Step {
  long time;
  byte lane;
  bool isHit;
};

enum GameState { WAITING, PLAYING, FINISHED };


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
  const int TIME_TRAVEL = 800;
  const int TOLERANCE = 200;
  int combo = 0;
  int lastWinStepIndex;

  void start() {
    _score = 0;
    _combo = 0;
    _lastWinStepIndex = -1;
    initializeSteps(); // On génère de nouvelles notes
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
      long lastNoteTime = _steps[_nSteps - 1].time;
      if (now() > lastNoteTime + TOLERANCE) {
        _state = FINISHED;
      }
    }
  }

  // on crée des steps avec une base de temps -> chaque step vient après l'autre dans le tableau
  void initializeSteps() {
    randomSeed(analogRead(A0));
    long timeBasis = 1000;
    for (int i = 0; i < _nSteps; i++) {
      timeBasis += random(200, 700);
      _steps[i].time = timeBasis;
      _steps[i].lane = random(0, 3);
      _steps[i].isHit = false;
    }
  }

  // permet de récupérer l'état du jeu
  GameState getState() { return _state; }

  // permet de mettre à jour l'état du jeu
  void setState(GameState s) { _state = s; }

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

  bool getStepIsHit(int i) {
    return _steps[i].isHit;
  }

  void setStepIsHit(int i, bool state) {
    _steps[i].isHit = state; 
  }

  // permet de récupérer le combo
  int getCombo() {
    return _combo;
  }

  // permet de donner une nouvelle valeur au combo
  void setCombo(int i) {
    _combo = i;
  }

  // permet de récupérer l'index du dernier step réussi
  int getLastWinStepIndex() {
    return _lastWinStepIndex;
  }

  // permet de définir l'index du dernier step réussi
  void setLastWinStepIndex(int i) {
    _lastWinStepIndex = i;
  }

  // permet de récupérer le score
  int getScore() {
    return _score;
  }

  // permet d'incrémenter le score
  void setScore(int i) {
    _score = _score + i;
  }
};

#endif