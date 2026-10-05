#include "include/snake.h"
// #include <algorithm>
#include <array>
#include <utility>

namespace s21::snake {

Snake::Snake(int head_x, int head_y, int length) : dir_(Direction::Right) {
  for (int i = 0; i < length; ++i) {
    body_.push_back({head_x - i, head_y});
  }
}
void Snake::Step() {
  static constexpr std::array<std::pair<int, int>, 4> Delta = {{
      {0, -1},  // up
      {1, 0},   // right
      {0, 1},   // down
      {-1, 0},  // left
  }};
  const auto [dx, dy] = Delta[static_cast<int>(dir_)];
  Block next_block{head().x + dx, head().y + dy};
  body_.push_front(next_block);
  if (grow_pending_) {
    grow_pending_ = 0;
  } else {
    body_.pop_back();
  }
}

void Snake::Grow() { grow_pending_ = 1; }

bool Snake::Contains(const Block& block) const {
  for (const auto& i : body_) {
    if (block == i) return 1;
  }
  return 0;
}

bool Snake::HitsSelf() const {
  const Block& head = body_.front();
  for (size_t i = 1; i < body_.size(); ++i) {
    if (body_[i] == head) return true;
  }
  return false;
}

}  // namespace s21::snake
