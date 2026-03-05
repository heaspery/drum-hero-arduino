#include "Leds.h"
#include "Game.h"
#include "Pitches.h"
#include "Buzzer.h"
#include "TouchSensor.h"

int value1 = 0;

Game game(50);

void setup() {
  Serial.begin(9600);
  initHardware();
  initTouchSensors();
  Serial.println("Système prêt");
}

void loop() {
  game.update();  // update game logic
  long currentTime = game.now();  // current game time (ms since start)

  switch (game.getState()) { 

    case WAITING:
      // wait for touch on sensor 0 to start
      if (isTouchHit(0)) {
        Serial.println("Let's go!");
        delay(200); // short delay to avoid double trigger
        game.start();
      }
      break;

    case PLAYING:
      // update leds and read the 3 touch sensors
      displayLeds(currentTime, game);
      for (int i = 0; i < 3; i++) {
        if (isTouchHit(i)) {
          game.checkInput(i, currentTime);
        }
      }
      break;

    case FINISHED:
      // display score then return to waiting state
      Serial.println(game.getScore());
      delay(3000);
      game.setState(WAITING);
      break;
  }
}