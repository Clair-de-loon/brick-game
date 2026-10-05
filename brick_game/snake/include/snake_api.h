#ifndef SNAKE_API_H
#define SNAKE_API_H

#include "../../brick_game.h"

#ifdef __cplusplus
extern "C" {
#endif

GameInfo_t snake_updateCurrentState(void);
void snake_userInput(UserAction_t action, bool hold);

#ifdef __cplusplus
}
#endif

#endif