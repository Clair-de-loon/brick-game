#ifndef TETRIS_OBJ_H
#define TETRIS_OBJ_H

#include "../brick_game.h"
typedef enum {  // fsm rows (start, spawn...x6)
  START = 0,
  SPAWN,
  MOVING,
  COLLIDE,
  GAME_OVER
} Tetris_state;

typedef struct {
  int type;       // O-piece, T-piece...
  int next_type;  // fot next piece preview
  int rotate;     // 0 1 2 3 rotations
  int x, y;       // figure left-up coordinates
} Current_figure;

typedef struct {  // GameInfo, Banner, current figure, tetris state
  GameInfo_t *game_info;
  Current_figure *cur_figure;
  Tetris_state state;
} Params_t;

typedef void (*action_func)(Params_t *prms);
void insertFigure(Params_t *prms, int erase);
int randomType();
void eraseLines(GameInfo_t *info);
int isCollide(Params_t *prms, int x, int y);
void updateNextField(Params_t *prms);
int tryMove(Params_t *prms, int x,
            int y);  // checks collide, clear and insert figure

// fsm action functions
void start(Params_t *prms);
// getters
action_func getFsmTable(int state, int action);
Params_t *getParamsPtr();
Current_figure *getCurFigurePtr();
UserAction_t getAction(int input);  // number to UserAction (27 to Terminate)

#endif