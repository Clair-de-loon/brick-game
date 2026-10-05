#ifndef SNAKE_H
#define SNAKE_H

#include <deque>

#include "types.h"

namespace s21::snake {
class Snake {
 private:
  std::deque<Block> body_;            // front()-head back()-tail
  Direction dir_ = Direction::Right;  // cur snake dir
  bool grow_pending_ = false;         //

 public:
  Snake(int head_x, int head_y, int length = 4);

  void SetDirection(Direction d) { dir_ = d; }
  void Step();                              // shift body
  void Grow();                              // eats food
  bool Contains(const Block& block) const;  // is block in body?
  bool HitsSelf() const;

  // getters
  const std::deque<Block>& body() const { return body_; }
  const Block& head() const { return body_.front(); }
  const Block& tail() const { return body_.back(); }
  Direction direction() const { return dir_; }
  int length() const { return static_cast<int>(body_.size()); }
};
}  // namespace s21::snake

#endif