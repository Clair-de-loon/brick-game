#ifndef MODEL_H
#define MODEL_H

#include "../../brick_game.h"
#include "board.h"
#include "fsm.h"
#include "snake.h"
#include "types.h"

namespace s21::snake {

class GameModel {
 private:
  void StartNewGame();  // init game data
  void UpdateHighScore();
  void UpdateLevel();
  int CurrentSpeed() const;
  static bool IsOpposite(Direction a, Direction b);

  Snake snake_;
  Board board_;
  SnakeFsm fsm_;
  GameState state_;
  Direction pending_dir_;
  int score_;
  int high_score_;
  int level_;
  int boost_ticks_;
  int win_length_;

 public:
  explicit GameModel(int win_length = 200);

  void Tick();
  void ApplyAction(UserAction_t act, bool hold);  // user input
  void FillGameInfo(GameInfo_t* out) const;
  void SetFood(const Block& b) { board_.SetFood(b); }

  static int LevelForScore(int score);
  static int SpeedForLevel(int level);

  GameState state() const { return state_; }
  int score() const { return score_; }
};

}  // namespace s21::snake
#endif