#include "tetris.h"

#include "tetris_obj.h"
#include "time.h"

typedef struct {
  int x;
  int y;
} Block_coords_t;  // mino xy

static const int figureArr[7][4][2] = {
    // 7 types 4 blocks 2 xy
    {{0, 0}, {0, 1}, {1, 0}, {1, 1}},  // O-piece
    {{0, 1}, {1, 1}, {2, 1}, {3, 1}},  // I-piece
    {{1, 0}, {1, 1}, {0, 1}, {2, 1}},  // T-piece
    {{1, 0}, {1, 1}, {1, 2}, {2, 2}},  // L-piece
    {{1, 0}, {1, 1}, {1, 2}, {0, 2}},  // J-piece
    {{0, 0}, {1, 0}, {1, 1}, {2, 1}},  // Z-piece
    {{1, 0}, {2, 0}, {1, 1}, {0, 1}},  // S-piece
};

static void getBlocksXY(Params_t *prms, Block_coords_t coords[4], int x, int y);

void tetris_userInput(UserAction_t action, bool hold) {
  Params_t *prms = getParamsPtr();
  hold += 0;  // заглушка

  int state = (int)prms->state;
  if (prms->game_info != NULL && prms->game_info->pause) {
    if ((int)action != 1 && (int)action != 2) {
      action = -1;
    }
  } else {
    if (prms->state == START) {
      start(prms);
      // automatic fsm (simulate press button)
    } else if (prms->state == SPAWN || prms->state == COLLIDE ||
               prms->state == GAME_OVER) {
      action = 0;  // enter
    }
  }
  if ((int)action != -1) {
    action_func cur_func = getFsmTable(state, action);
    if (cur_func != NULL) {
      cur_func(prms);
    }
  }
}

int randomType() {  // random type of tetramino
  return rand() % 7;
}

Params_t *getParamsPtr() {
  static Params_t params = {0};
  return &params;
}

Current_figure *getCurFigurePtr() {
  static Current_figure figure = {0};
  return &figure;
}

// UPDATECURRENTSTATE
GameInfo_t tetris_updateCurrentState() {
  Params_t *prms = getParamsPtr();
  GameInfo_t *game_info = getGameInfoPtr();

  if (!game_info->pause && prms->state == MOVING) {
    if (!tryMove(prms, 0, 1)) {
      prms->state = COLLIDE;
    }
  }
  return *game_info;
}

void insertFigure(Params_t *prms, int erase) {
  int type = prms->cur_figure->type;
  Block_coords_t coords[4] = {0};
  getBlocksXY(prms, coords, 0, 0);

  for (int i = 0; i < 4; i++) {
    if (coords[i].y >= 0 && coords[i].y < 20 && coords[i].x >= 0 &&
        coords[i].x < 10) {
      prms->game_info->field[coords[i].y][coords[i].x] = erase ? 0 : type + 1;
    }
  }
}
void updateNextField(Params_t *prms) {
  int next_type = prms->cur_figure->next_type;
  int **next_matrix = prms->game_info->next;

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      next_matrix[i][j] = 0;
    }
  }
  for (int i = 0; i < 4; i++) {
    int local_x = figureArr[next_type][i][0];
    int local_y = figureArr[next_type][i][1];
    next_matrix[local_y][local_x] = next_type + 1;
  }
}

static void getBlocksXY(Params_t *prms, Block_coords_t coords[4], int x,
                        int y) {
  int figure_x = prms->cur_figure->x + x;
  int figure_y = prms->cur_figure->y + y;
  int type = prms->cur_figure->type;
  int rotate = prms->cur_figure->rotate;

  int size = 3;             // local figure size
  if (type == 0) size = 2;  // O-piece type
  if (type == 1) size = 4;  // I-piece type

  for (int i = 0; i < 4; i++) {
    int local_x = figureArr[type][i][0];
    int local_y = figureArr[type][i][1];
    for (int j = 0; j < rotate; j++) {
      int temp_x = size - 1 - local_y;  // matrix rotate formula
      int temp_y = local_x;
      local_x = temp_x;
      local_y = temp_y;
    }
    coords[i].x = figure_x + local_x;
    coords[i].y = figure_y + local_y;
  }
}

int isCollide(Params_t *prms, int x, int y) {
  int res = 0;
  Block_coords_t coords[4] = {0};
  getBlocksXY(prms, coords, x, y);
  int **field = prms->game_info->field;
  int i = 0, flag = 1;

  while (i < 4 && flag) {
    if (coords[i].x < 0 || coords[i].x >= 10 || coords[i].y >= 20) {
      res = 1;
      flag = 0;
    } else if (coords[i].y >= 0 && field[coords[i].y][coords[i].x] != 0) {
      res = 1;
      flag = 0;
    }
    i++;
  }
  return res;
}
int tryMove(Params_t *prms, int x, int y) {
  int res = 0;
  insertFigure(prms, 1);  // erase
  if (!isCollide(prms, x, y)) {
    prms->cur_figure->x += x;
    prms->cur_figure->y += y;
    res = 1;
  }
  insertFigure(prms, 0);
  return res;
}
void eraseLines(GameInfo_t *info) {
  int **field = info->field;
  int erasedLines = 0;

  for (int i = 19; i >= 0; i--) {
    int flag = 1;
    int is_row_full = 1;

    // find full row
    for (int j = 0; j < 10 && flag; j++) {
      if (field[i][j] == 0) {
        is_row_full = 0;
        flag = 0;
      }
    }
    if (is_row_full) {
      erasedLines++;
      // shift upper rows to lower (18 to 17)
      for (int k = i; k > 0; k--) {
        for (int j = 0; j < 10; j++) field[k][j] = field[k - 1][j];
      }
      // new clear row
      for (int j = 0; j < 10; j++) {
        field[0][j] = 0;
      }
      i++;
    }
  }
  if (erasedLines >= 1) scoring(info, erasedLines);
}

void scoring(GameInfo_t *info, int lines) {
  if (lines >= 4) {
    info->score += 1500;
  } else if (lines == 3) {
    info->score += 700;
  } else if (lines == 2) {
    info->score += 300;
  } else if (lines == 1) {
    info->score += 100;
  }
}