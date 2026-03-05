#ifndef GAME_H
#define GAME_H

#include <Arduino.h>
#include <Chrono.h>

// one game step (time + lane + result flags)
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
  int _score = 0;
  int _nextCheckIndex = 0;

  int _nextSoundStepIndex = 0;

public:

  // hit window in ms (+/- around step time)
  const int TOLERANCE = 500; // CHANGE ME IF YOU WANT TO ADAPT THE GAME DIFFICULTY

  Game(int n)
    : _nSteps(n) {
    _steps = new Step[_nSteps];
  }

  ~Game() {
    delete[] _steps;
  }

  // reset and start a new game
  void start() {
    _score = 0;
    _combo = 0;
    _nextCheckIndex = 0;
    _nextSoundStepIndex = 0;
    initializeSteps();
    _state = PLAYING;
    _myChrono.restart();
  }

  // random steps, spaced by 400ms
  void initializeSteps() {
    randomSeed(analogRead(A5));
    long timeBasis = 1000;

    for (int i = 0; i < _nSteps; i++) {
      timeBasis += 400; // CHANGE ME IF YOU WANT TO ADAPT THE GAME DIFFICULTY

      _steps[i].time = timeBasis;
      _steps[i].lane = random(3);
      _steps[i].isHit = false;
      _steps[i].isHandled = false;
    }
  }


  void update() {
    sendGameState();
    if (_state == PLAYING) {
      // mark missed steps
      checkMissed();

      long t = now();

      // advance sound index (skip past steps)
      while (_nextSoundStepIndex < _nSteps && t >= _steps[_nextSoundStepIndex].time) {
        _nextSoundStepIndex++;
      }

      // end when last step is out of tolerance
      if (t > _steps[_nSteps - 1].time + TOLERANCE) {
        delay(2000); // let the player see the score before reset
        _state = FINISHED;
        _score = 0;
      }
    }
  }

  // check if this lane matches a step within the hit window
  void checkInput(int lane, long now) {
    for (int i = _nextCheckIndex; i < _nSteps; i++) {
      if (_steps[i].lane == lane && !_steps[i].isHandled) {
        long diff = labs(now - _steps[i].time);

        if (diff <= TOLERANCE) {
          _score++;
          _steps[i].isHit = true;
          _steps[i].isHandled = true;
          _combo++;

          Serial.println("SUCCESS");
          return;
        }
        if (_steps[i].time > now + 1000) break;
      }
    }
  }

  // if a step is too late and still unhandled -> MISS + combo reset
  void checkMissed() {
    long currentTime = now();
    while (_nextCheckIndex < _nSteps && currentTime > (_steps[_nextCheckIndex].time + TOLERANCE)) {
      if (!_steps[_nextCheckIndex].isHandled) {
        _steps[_nextCheckIndex].isHandled = true;
        _steps[_nextCheckIndex].isHit = false;
        _combo = 0;
        Serial.println("MISS");
      }
      _nextCheckIndex++;
    }
  }

  // send current game state (JSON over Serial)
  void sendGameState() {
    Serial.print("{\"state\":");
    Serial.print(_state);
    Serial.print(",\"score\":");
    Serial.print(_score);
    Serial.print(",\"combo\":");
    Serial.print(_combo);
    Serial.print(",\"nextCheckIndex\":");
    Serial.print(_nextCheckIndex);
    Serial.println("}");
  }

  // getters
  GameState getState() {
    return _state;
  }
  void setState(GameState s) {
    _state = s;
  }
  long now() {
    return _myChrono.elapsed();
  }
  int getNSteps() {
    return _nSteps;
  }
  long getStepTime(int i) {
    return _steps[i].time;
  }
  int getStepLane(int i) {
    return _steps[i].lane;
  }
  int getScore() {
    return _score;
  }
};

#endif