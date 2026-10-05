#include "brick_game.h"

GameInfo_t *getGameInfoPtr() {        // reterns GameInfo ptr
  static GameInfo_t game_info = {0};  // singleton
  return &game_info;
}

void freeGameInfo(GameInfo_t *game_info) {
  if (game_info->field) {
    for (int i = 0; i < 20; i++) {
      free(game_info->field[i]);
    }
    free(game_info->field);
    game_info->field = NULL;
  }

  if (game_info->next) {
    for (int i = 0; i < 4; i++) {
      free(game_info->next[i]);
    }
    free(game_info->next);
    game_info->next = NULL;
  }
}

int initGameInfo() {  // lll
  int status = 0;
  GameInfo_t *game_info = getGameInfoPtr();

  game_info->next = calloc(4, sizeof(int *));
  for (int i = 0; i < 4; i++) game_info->next[i] = calloc(4, sizeof(int));

  game_info->field = calloc(20, sizeof(int *));
  for (int j = 0; j < 20; j++) {
    game_info->field[j] = calloc(10, sizeof(int));
  }

  if (game_info->next == NULL || game_info->field == NULL) {
    status = 1;
  }
  game_info->high_score = 0;
  game_info->level = 0;
  game_info->pause = 0;
  game_info->score = 0;
  game_info->speed = 1000;  // lvl 1 speed
  return status;
}
