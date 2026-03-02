#ifndef GAME_H
#define GAME_H

#include <Arduino.h>
#include <Chrono.h>
#include "buzzer.h"
#include "pitches.h"

struct Step {
  long time;
  byte lane;
  int pitch; 
  bool isHit;
  bool isHandled;
};

enum GameState { WAITING, PLAYING, FINISHED };

class Game {
private:
  GameState _state = WAITING;
  int _nSteps;
  Step* _steps;
  Chrono _myChrono;
  int _combo = 0;
  int _score = 0;
  int _nextCheckIndex = 0;

  const int tetrisMelody[25] = {
    NOTE_E5, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_C5, NOTE_B4, NOTE_A4, // Phrase 1
    NOTE_A4, NOTE_C5, NOTE_E5, NOTE_D5, NOTE_C5, NOTE_B4,          // Phrase 2
    NOTE_C5, NOTE_D5, NOTE_E5, NOTE_C5, NOTE_A4, NOTE_A4,          // Phrase 3
    NOTE_D5, NOTE_F5, NOTE_A5, NOTE_G5, NOTE_F5, NOTE_E5           // Phrase 4
  };

public:
  // tolerance en ms 
  const int TOLERANCE = 500;

  Game(int n) : _nSteps(n) {
    _steps = new Step[_nSteps];
  }

  ~Game() {
    delete[] _steps;
  }

  void start() {
    _score = 0;
    _combo = 0;
    _nextCheckIndex = 0;
    initializeSteps(); 
    _state = PLAYING;
    _myChrono.restart();
  }

  // Initialisation avec la mélodie 
  void initializeSteps() {
    randomSeed(analogRead(A5));
    long timeBasis = 1000;
    
    for (int i = 0; i < _nSteps; i++) {
      timeBasis += 400; 
      
      _steps[i].time = timeBasis;
      _steps[i].lane = random(3);
      _steps[i].isHit = false;
      _steps[i].isHandled = false;
      
      // On boucle sur le tableau tetrisMelody pour remplir les steps
      _steps[i].pitch = tetrisMelody[i % 25]; 
    }
  }

  void update() {
    if (_state == PLAYING) {
      checkMissed();
      if (now() > _steps[_nSteps - 1].time + TOLERANCE) {
        _state = FINISHED;
      }
    }
  }

  void checkInput(int lane, long now) {
    for (int i = _nextCheckIndex; i < _nSteps; i++) {
      if (_steps[i].lane == lane && !_steps[i].isHandled) {
        long diff = labs(now - _steps[i].time);
        
        if (diff <= TOLERANCE) {
          _score++;
          _steps[i].isHit = true;
          _steps[i].isHandled = true;
          _combo++;
          
          // Joue la note spécifique associée à ce Step
          playNote(_steps[i].pitch, 150); 
          
          Serial.println("SUCCESS");
          return; 
        }
        if (_steps[i].time > now + 1000) break; 
      }
    }
  }

  void checkMissed() {
    long currentTime = now();
    while (_nextCheckIndex < _nSteps && currentTime > (_steps[_nextCheckIndex].time + TOLERANCE)) {
        if (!_steps[_nextCheckIndex].isHandled) {
            _steps[_nextCheckIndex].isHandled = true;
            _steps[_nextCheckIndex].isHit = false;
            _combo = 0;
        }
        _nextCheckIndex++;
    }
  }

  // Getters
  GameState getState() { return _state; }
  void setState(GameState s) { _state = s; }
  long now() { return _myChrono.elapsed(); }
  int getNSteps() { return _nSteps; }
  long getStepTime(int i) { return _steps[i].time; }
  int getStepLane(int i) { return _steps[i].lane; }
  int getScore() { return _score; }
  int getStepPitch(int i) { return _steps[i].pitch; }
};

#endif