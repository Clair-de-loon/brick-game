#include "include/board.h"

#include <vector>

#include "include/types.h"

namespace s21::snake {
bool Board::IsInside(const Block& b) const {
  return b.x >= 0 && b.x < Width && b.y >= 0 && b.y < Height;
}

void Board::SpawnFood(const Snake& snake) {
  std::vector<Block> free_blocks;  // empty blocks on board
  free_blocks.reserve(10 * 20);

  for (int i = 0; i < Width; i++) {
    for (int j = 0; j < Height; j++) {
      Block c{i, j};
      if (!snake.Contains(c)) free_blocks.push_back(c);
    }
  }

  if (free_blocks.empty()) {
    // win
    return;
  }
  std::uniform_int_distribution<size_t> pick(0, free_blocks.size() - 1);
  food_ = free_blocks[pick(rng_)];
}

}  // namespace s21::snake