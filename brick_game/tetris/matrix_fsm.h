#ifndef MATRIX_FSM_H
#define MATRIX_FSM_H
#include "../brick_game.h"
#include "tetris_obj.h"

// Params_t getParams();
void spawn(Params_t *prms);
void rotate(Params_t *prms);
void pause(Params_t *prms);
void move_down(Params_t *prms);
void move_right(Params_t *prms);
void move_left(Params_t *prms);
void collide(Params_t *prms);
void game_over(Params_t *prms);

#endif