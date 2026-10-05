#include "include/fsm.h"

namespace s21::snake {

using enum GameState;
// clang-format off
//                Start               Pause          Terminate      Left           Right        Up           Down               Action
const GameState SnakeFsm::fsm[6][8] = {
    /* Start    */ {Spawn,    Start,    Start,    Start,    Start,   Start,   Start,   Start},
    /* Spawn    */ {Spawn,    Spawn,    Spawn,    Spawn,    Spawn,   Spawn,   Spawn,   Spawn},
    /* Moving   */ {Moving,   Paused,   GameOver, Moving,   Moving,  Moving,  Moving,  Moving},
    /* Collide  */ {GameOver, Collide,  GameOver, Collide,  Collide, Collide, Collide, Collide},
    /* kPaused   */ {Paused,   Moving,   GameOver, Paused,   Paused,  Paused,  Paused,  Paused},
    /* GameOver */ {Start,    GameOver, GameOver, GameOver, GameOver,GameOver,GameOver,GameOver},
};
// clang-format on
GameState SnakeFsm::NextState(GameState current, UserAction_t action) const {
  int s = static_cast<int>(current);
  int a = static_cast<int>(action);
  if (s < 0 || s >= 6 || a < 0 || a >= 8) return current;
  return fsm[s][a];
}

}  // namespace s21::snake