#include "matrix_fsm.h"

#include <stdio.h>

// #define NULL ((void *)0)

// not sure about nosig

//         Enter  Pause Esc   Left   Right    Up    Down  Space
////////////////////////////////////////////////////////////////////
// start   | spawn  null  null  null   null   null   null  null
// spawn   | spawn  null  null  null   null   null   null  null
// moving  | null   pause null  mleft  mright null   mdown rotate
// collide | coll   exit  null   null   null   null  null  null
// gameover| gamov  exit  null   null   null   null   null  null

action_func fsm_table[5][8] = {
    {spawn, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {spawn, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {NULL, pause, NULL, move_left, move_right, NULL, move_down, rotate},
    {collide, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    {game_over, NULL, NULL, NULL, NULL, NULL, NULL, NULL}};

action_func getFsmTable(int state, int action) {
  return fsm_table[state][action];
}
UserAction_t getAction(int input) {
  UserAction_t act = -1;  // nosig
  if (input == 0403) {    // move up
    act = Up;
  } else if (input == 0402) {  // move down
    act = Down;
  } else if (input == 0404) {  // move left
    act = Left;
  } else if (input == 0405) {  // move right
    act = Right;
  } else if (input == 27) {  // esc
    act = Terminate;
  } else if (input == 80 || input == 112) {  // pP
    act = Pause;
  } else if (input == 32) {  // space
    act = Action;
  } else if (input == 10) {  // enter
    act = Start;
  }
  return act;
}

void start(Params_t *prms) {
  prms->game_info = getGameInfoPtr();
  // get record from file
  FILE *file = fopen("tetris_record.txt", "r");
  if (file) {
    int record = 0;
    if (fscanf(file, "%d", &record) == 1) {
      prms->game_info->high_score = record;
    }
  }
  // init tetris
  prms->state = 0;
  prms->cur_figure = getCurFigurePtr();
  prms->cur_figure->type = randomType();
  prms->cur_figure->next_type = randomType();
}

void spawn(Params_t *prms) {
  int calculated_level = 1 + (prms->game_info->score / 600);
  if (calculated_level > 10) {
    calculated_level = 10;
  }
  prms->game_info->level = calculated_level;
  prms->cur_figure->rotate = 0;
  prms->cur_figure->x = 3;
  prms->cur_figure->y = 0;
  prms->cur_figure->type = prms->cur_figure->next_type;
  prms->cur_figure->next_type = randomType();
  updateNextField(prms);

  insertFigure(prms, 0);
  prms->state = MOVING;
}

void rotate(Params_t *prms) {
  int old_rotate = prms->cur_figure->rotate;
  insertFigure(prms, 1);  // clear

  prms->cur_figure->rotate = (old_rotate + 1) % 4;
  if (isCollide(prms, 0, 0)) {
    prms->cur_figure->rotate = old_rotate;  // dont rotate
  }
  insertFigure(prms, 0);
}

void pause(Params_t *prms) {
  prms->game_info->pause = !prms->game_info->pause;
  if (prms->game_info->pause == 0) {
    prms->state = MOVING;
  }
}

void move_down(Params_t *prms) {
  if (!tryMove(prms, 0, 1)) {
    prms->state = COLLIDE;
  }
}
void move_right(Params_t *prms) { tryMove(prms, 1, 0); }

void move_left(Params_t *prms) { tryMove(prms, -1, 0); }

void collide(Params_t *prms) {
  if (prms->cur_figure->y <= 0) {
    prms->state = GAME_OVER;
  } else {
    eraseLines(prms->game_info);
    prms->state = SPAWN;
  }
}

void game_over(Params_t *prms) {
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10; j++) {
      prms->game_info->field[i][j] = 0;
    }
  }
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      prms->game_info->next[i][j] = 0;
    }
  }
  // save new record
  if (prms->game_info->score > prms->game_info->high_score) {
    FILE *file = fopen("tetris_record.txt", "w");
    if (file) {
      fprintf(file, "%d", prms->game_info->score);
      fclose(file);
    }
  }
  prms->game_info->score = 0;
  prms->state = START;
}