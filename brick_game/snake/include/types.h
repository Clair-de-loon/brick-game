#ifndef TYPES_H
#define TYPES_H

#include "../../brick_game.h"

namespace s21::snake {

enum class GameState { Start = 0, Spawn, Moving, Collide, Paused, GameOver };

enum class Direction { Up = 0, Right, Down, Left };

struct Block {
  int x = 0;
  int y = 0;
  bool operator==(const Block& block) const {
    return x == block.x && y == block.y;
  }
  bool operator!=(const Block& block) const {
    return x != block.x && y != block.y;
  }
};

}  // namespace s21::snake
#endif