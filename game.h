#ifndef GAME_H
#define GAME_H

#include <Arduino.h>
#include <Chrono.h>

class Game {
private:
  int _nSteps;
  long (*_steps)[2]; 
  Chrono _myChrono;
  
public:
  const int TIME_TRAVEL = 800;
  const int TOLERANCE = 200;

  Game(int n) : _nSteps(n) {
    _steps = new long[_nSteps][2];
    initializeSteps();
  }

  ~Game() {
    delete[] _steps;
  }

  void initializeSteps() {
    randomSeed(analogRead(A0));
    long timeBasis = 1000;
    for (int i = 0; i < _nSteps; i++) {
      timeBasis += random(200, 700);
      _steps[i][0] = timeBasis;
      _steps[i][1] = random(0, 3); 
    }
  }

  void restart() { _myChrono.restart(); }
  long now() { return _myChrono.elapsed(); }
  
  int getNSteps() { return _nSteps; }
  long getStepTime(int i) { return _steps[i][0]; }
  int getStepLane(int i) { return _steps[i][1]; }
};

#endif