#include "include/model.h"

#include "include/board.h"
#include "include/snake.h"
#include "include/types.h"

namespace s21::snake {

namespace {
constexpr int InitLength = 4;
//constexpr int WinLength = 200;
constexpr int BaseSpeed = 350;
constexpr int MinSpeed = 60;
constexpr int MaxLevel = 10;
constexpr int BoostTicks = 3;
// frontend colors
constexpr int EmptyCell = 0;
constexpr int SnakeCell = 1;
constexpr int HeadCell = 5;
constexpr int FoodCell = 6;
constexpr const char* kRecordPath = "snake_record.txt";

int LoadRecord() {
  std::FILE* f = std::fopen(kRecordPath, "r");
  if (!f) return 0;
  int value = 0;
  if (std::fscanf(f, "%d", &value) != 1) value = 0;
  std::fclose(f);
  return value < 0 ? 0 : value;
}

void SaveRecord(int score) {
  std::FILE* f = std::fopen(kRecordPath, "w");
  if (!f) return;
  std::fprintf(f, "%d\n", score);
  std::fclose(f);
}

}  // namespace

GameModel::GameModel(int win_length)
    : snake_(Board::Width / 2, Board::Height / 2, InitLength),
      state_(GameState::Start),
      pending_dir_(Direction::Right),
      score_(0),
      high_score_(LoadRecord()),
      level_(1),
      boost_ticks_(0),
      win_length_(win_length) {}

void GameModel::StartNewGame() {
  snake_ = Snake(5, 10, InitLength);
  board_.SpawnFood(snake_);
  pending_dir_ = Direction::Right;
  score_ = 0;
  boost_ticks_ = 0;
  level_ = 1;
}

void GameModel::ApplyAction(UserAction_t act, bool hold) {
  (void)hold;

  const GameState next = fsm_.NextState(state_, act);
  if (state_ == GameState::Start && next == GameState::Spawn) {
    StartNewGame();
  }
  if (state_ == GameState::Moving || state_ == GameState::Paused) {
    Direction want = pending_dir_;
    bool is_direction = true;
    switch (act) {
      case Up:
        want = Direction::Up;
        break;
      case Down:
        want = Direction::Down;
        break;
      case Left:
        want = Direction::Left;
        break;
      case Right:
        want = Direction::Right;
        break;
      default:
        is_direction = false;
        break;
    }
    if (is_direction && !IsOpposite(want, pending_dir_)) {
      pending_dir_ = want;
    }
    if (act == Action && state_ == GameState::Moving) {
      boost_ticks_ = BoostTicks;
    }
  }
  state_ = next;
}
void GameModel::Tick() {
  if (state_ == GameState::Spawn) {
    state_ = GameState::Moving;
    return;
  }
  if (state_ == GameState::Collide) {
    state_ = GameState::GameOver;
    return;
  }
  if (state_ != GameState::Moving) return;

  snake_.SetDirection(pending_dir_);
  snake_.Step();

  const Block head = snake_.head();
  if (!board_.IsInside(head) || snake_.HitsSelf()) {
    state_ = GameState::Collide;
    return;
  }
  if (board_.EatFoodAt(head)) {
    snake_.Grow();
    ++score_;
    UpdateHighScore();
    UpdateLevel();
    if (snake_.length() + 1 >= win_length_) {
      state_ = GameState::GameOver;
      return;
    }
    board_.SpawnFood(snake_);
  }
  if (boost_ticks_ > 0) --boost_ticks_;
}
void GameModel::UpdateLevel() { level_ = LevelForScore(score_); }

void GameModel::UpdateHighScore() {
  if (score_ > high_score_) {
    high_score_ = score_;
    SaveRecord(high_score_);
  }
}

int GameModel::CurrentSpeed() const {
  int speed = SpeedForLevel(level_);
  if (boost_ticks_ > 0) speed /= 2;
  return speed;
}

bool GameModel::IsOpposite(Direction a, Direction b) {
  return (static_cast<int>(a) + 2) % 4 == static_cast<int>(b);
}
int GameModel::SpeedForLevel(int level) {
  if (level < 1) level = 1;
  if (level > MaxLevel) level = MaxLevel;
  const int range = BaseSpeed - MinSpeed;
  int speed = BaseSpeed - (level - 1) * range / (MaxLevel - 1);
  if (speed < MinSpeed) speed = MinSpeed;
  if (speed > BaseSpeed) speed = BaseSpeed;
  return speed;
}

int GameModel::LevelForScore(int score) {
  if (score < 0) score = 0;
  int level = 1 + score / 5;
  if (level > MaxLevel) level = MaxLevel;
  return level;
}

void GameModel::FillGameInfo(GameInfo_t* out) const {
  for (int i = 0; i < 20; ++i) {
    for (int j = 0; j < 10; ++j) {
      out->field[i][j] = EmptyCell;
    }
  }
  if (state_ == GameState::Moving || state_ == GameState::Paused) {
    const Block& food = board_.food();
    out->field[food.y][food.x] = FoodCell;

    for (auto& item : snake_.body()) {
      if (board_.IsInside(item)) {
        out->field[item.y][item.x] = SnakeCell;
      }
    }
    const Block& head = snake_.head();
    if (board_.IsInside(head)) {
      out->field[head.y][head.x] = HeadCell;
    }
  }
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      out->next[i][j] = 0;
    }
  }
  out->score = score_;
  out->high_score = high_score_;
  out->level = 1 + score_ / 5;
  out->speed = CurrentSpeed();
  out->pause = (state_ == GameState::Paused) ? 1 : 0;
}

}  // namespace s21::snake