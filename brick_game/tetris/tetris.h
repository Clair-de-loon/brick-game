#ifndef TETRIS_H
#define TETRIS_H

#include <stdio.h>  //for fopen debug !!!!
#include <stdlib.h>

// #include "../brick_game.h"
#include "matrix_fsm.h"

void scoring(GameInfo_t *info, int lines);
GameInfo_t tetris_updateCurrentState(void);
void tetris_userInput(UserAction_t action, bool hold);

#endif