#ifndef BOARD_H
#define BOARD_H

#include <random>

#include "snake.h"
#include "types.h"
namespace s21::snake {
class Board {
 private:
  Block food_{5, 10};  // first spawn at center
  std::mt19937 rng_{std::random_device{}()};

 public:
  static constexpr int Width = 10;
  static constexpr int Height = 20;

  Board() = default;

  bool IsInside(const Block& b) const;  // is block inside board
  void SpawnFood(const Snake& snake);
  bool EatFoodAt(const Block& block) const { return block == food_; }
  void SetFood(const Block& b) { food_ = b; }
  // getters
  int width() const { return Width; }
  int height() const { return Height; }
  const Block& food() const { return food_; }
};
}  // namespace s21::snake

#endif