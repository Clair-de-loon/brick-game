#ifndef BRICK_GAME_H
#define BRICK_GAME_H

#include "stdbool.h"
#include "stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {  // game fields, scores, lvl, speed, pause
  int **field;
  int **next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

typedef enum {  // start, pause, terminate...x8
  Start = 0,    // enter
  Pause,        // p btn
  Terminate,    // esc
  Left,
  Right,
  Up,
  Down,
  Action  // space
} UserAction_t;

typedef enum { kTetris = 0, kSnake = 1 } GameType;

GameInfo_t *getGameInfoPtr();
GameInfo_t updateCurrentState();

void freeGameInfo(GameInfo_t *game_info);
int initGameInfo();
UserAction_t getAction(int input);  // convert num to action

void userInput(UserAction_t action,
               bool hold);  // own function for each brick game

void selectGame(GameType game);

#ifdef __cplusplus
}
#endif
#endif