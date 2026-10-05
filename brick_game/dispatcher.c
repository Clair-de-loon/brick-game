#include "brick_game.h"
#include "snake/include/snake_api.h"
#include "tetris/tetris.h"

static GameType g_current_game = kTetris;

void selectGame(GameType game) { g_current_game = game; }

GameInfo_t updateCurrentState(void) {
  if (g_current_game == kSnake) return snake_updateCurrentState();
  return tetris_updateCurrentState();
}

void userInput(UserAction_t action, bool hold) {
  if (g_current_game == kSnake)
    snake_userInput(action, hold);
  else
    tetris_userInput(action, hold);
}