#ifndef FSM_H
#define FSM_H

#include "../../brick_game.h"
#include "types.h"

namespace s21::snake {
class SnakeFsm {
 private:
  static const GameState fsm[6][8];

 public:
  SnakeFsm() = default;

  GameState NextState(GameState cur, UserAction_t act) const;
};

}  // namespace s21::snake

#endif