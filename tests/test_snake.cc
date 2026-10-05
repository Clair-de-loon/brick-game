#include <gtest/gtest.h>

#include "../brick_game/snake/include/board.h"
#include "../brick_game/snake/include/fsm.h"
#include "../brick_game/snake/include/model.h"
#include "../brick_game/snake/include/snake.h"

using s21::snake::Block;
using s21::snake::Board;
using s21::snake::Direction;
using s21::snake::GameModel;
using s21::snake::GameState;
using s21::snake::Snake;
using s21::snake::SnakeFsm;

class FsmTest : public ::testing::Test {
 protected:
  SnakeFsm fsm;
};

TEST_F(FsmTest, StartOnEnterGoesToSpawn) {
  EXPECT_EQ(fsm.NextState(GameState::Start, Start), GameState::Spawn);
}

TEST_F(FsmTest, StartIgnoresEverythingElse) {
  for (auto a : {Pause, Terminate, Left, Right, Up, Down, Action}) {
    EXPECT_EQ(fsm.NextState(GameState::Start, a), GameState::Start);
  }
}

TEST_F(FsmTest, SpawnStaysForAnyAction) {
  for (auto a : {Start, Pause, Terminate, Left, Right, Up, Down, Action}) {
    EXPECT_EQ(fsm.NextState(GameState::Spawn, a), GameState::Spawn);
  }
}

TEST_F(FsmTest, MovingOnPauseGoesToPaused) {
  EXPECT_EQ(fsm.NextState(GameState::Moving, Pause), GameState::Paused);
}

TEST_F(FsmTest, MovingOnTerminateGoesToGameOver) {
  EXPECT_EQ(fsm.NextState(GameState::Moving, Terminate), GameState::GameOver);
}

TEST_F(FsmTest, MovingStaysOnArrowsAndAction) {
  for (auto a : {Left, Right, Up, Down, Action, Start}) {
    EXPECT_EQ(fsm.NextState(GameState::Moving, a), GameState::Moving);
  }
}

TEST_F(FsmTest, CollideOnEnterGoesToGameOver) {
  EXPECT_EQ(fsm.NextState(GameState::Collide, Start), GameState::GameOver);
}

TEST_F(FsmTest, CollideStaysOnEverythingElse) {
  for (auto a : {Pause, Left, Right, Up, Down, Action}) {
    EXPECT_EQ(fsm.NextState(GameState::Collide, a), GameState::Collide);
  }
}

TEST_F(FsmTest, PausedOnPauseReturnsToMoving) {
  EXPECT_EQ(fsm.NextState(GameState::Paused, Pause), GameState::Moving);
}

TEST_F(FsmTest, PausedOnTerminateGoesToGameOver) {
  EXPECT_EQ(fsm.NextState(GameState::Paused, Terminate), GameState::GameOver);
}

TEST_F(FsmTest, PausedStaysOnEverythingElse) {
  for (auto a : {Left, Right, Up, Down, Action, Start}) {
    EXPECT_EQ(fsm.NextState(GameState::Paused, a), GameState::Paused);
  }
}

TEST_F(FsmTest, GameOverOnEnterGoesToStart) {
  EXPECT_EQ(fsm.NextState(GameState::GameOver, Start), GameState::Start);
}

TEST_F(FsmTest, GameOverStaysOnEverythingElse) {
  for (auto a : {Pause, Terminate, Left, Right, Up, Down, Action}) {
    EXPECT_EQ(fsm.NextState(GameState::GameOver, a), GameState::GameOver);
  }
}

TEST_F(FsmTest, OutOfRangeReturnsCurrent) {
  EXPECT_EQ(fsm.NextState(GameState::Moving, static_cast<UserAction_t>(999)),
            GameState::Moving);
}

TEST(SnakeTest, InitialLengthAndHead) {
  Snake s(5, 10, 4);
  EXPECT_EQ(s.length(), 4);
  EXPECT_EQ(s.head(), (Block{5, 10}));
  EXPECT_EQ(s.body().back(), (Block{2, 10}));
  EXPECT_EQ(s.direction(), Direction::Right);
}

TEST(SnakeTest, StepMovesHeadRight) {
  Snake s(5, 10, 4);
  s.Step();
  EXPECT_EQ(s.head(), (Block{6, 10}));
  EXPECT_EQ(s.length(), 4);
}

TEST(SnakeTest, StepInAllDirections) {
  Snake up(5, 10, 1);
  up.SetDirection(Direction::Up);
  up.Step();
  EXPECT_EQ(up.head(), (Block{5, 9}));

  Snake down(5, 10, 1);
  down.SetDirection(Direction::Down);
  down.Step();
  EXPECT_EQ(down.head(), (Block{5, 11}));

  Snake left(5, 10, 1);
  left.SetDirection(Direction::Left);
  left.Step();
  EXPECT_EQ(left.head(), (Block{4, 10}));

  Snake right(5, 10, 1);
  right.SetDirection(Direction::Right);
  right.Step();
  EXPECT_EQ(right.head(), (Block{6, 10}));
}

TEST(SnakeTest, GrowAddsSegmentOnNextStep) {
  Snake s(5, 10, 4);
  s.Grow();
  s.Step();
  EXPECT_EQ(s.length(), 5);
  s.Step();
  EXPECT_EQ(s.length(), 5);
}

TEST(SnakeTest, Contains) {
  Snake s(5, 10, 4);
  EXPECT_TRUE(s.Contains({5, 10}));
  EXPECT_TRUE(s.Contains({2, 10}));
  EXPECT_FALSE(s.Contains({7, 10}));
}

TEST(SnakeTest, HitsSelf) {
  Snake s(5, 10, 5);
  s.SetDirection(Direction::Up);
  s.Step();
  s.SetDirection(Direction::Left);
  s.Step();
  s.SetDirection(Direction::Down);
  s.Step();
  EXPECT_TRUE(s.HitsSelf());
}

TEST(BoardTest, IsInsideBounds) {
  Board b;
  EXPECT_TRUE(b.IsInside({0, 0}));
  EXPECT_TRUE(b.IsInside({9, 19}));
  EXPECT_FALSE(b.IsInside({-1, 0}));
  EXPECT_FALSE(b.IsInside({10, 0}));
  EXPECT_FALSE(b.IsInside({0, 20}));
}

TEST(BoardTest, EatFoodAtReturnsTrueOnlyForFood) {
  Snake s(5, 10, 4);
  Board b;
  b.SpawnFood(s);
  EXPECT_TRUE(b.EatFoodAt(b.food()));
  EXPECT_FALSE(b.EatFoodAt({0, 0}));
}

TEST(BoardTest, FoodIsUnique) {
  Snake s(5, 10, 4);
  Board b;
  for (int i = 0; i < 50; ++i) {
    b.SpawnFood(s);
    EXPECT_FALSE(s.Contains(b.food()));
    EXPECT_TRUE(b.IsInside(b.food()));
  }
}

TEST(ModelTest, EnterStartsGameAndGoesToSpawn) {
  GameModel m;
  m.ApplyAction(Start, false);
  EXPECT_EQ(m.state(), GameState::Spawn);
}

TEST(ModelTest, SpawnGoesToMoving) {
  GameModel m;
  m.ApplyAction(Start, false);
  m.Tick();
  EXPECT_EQ(m.state(), GameState::Moving);
}

TEST(ModelTest, PauseAndResume) {
  GameModel m;
  m.ApplyAction(Start, false);
  m.Tick();
  m.ApplyAction(Pause, false);
  EXPECT_EQ(m.state(), GameState::Paused);
  m.ApplyAction(Pause, false);
  EXPECT_EQ(m.state(), GameState::Moving);
}

TEST(ModelTest, TerminateGoesToGameOver) {
  GameModel m;
  m.ApplyAction(Start, false);
  m.Tick();
  m.ApplyAction(Terminate, false);
  EXPECT_EQ(m.state(), GameState::GameOver);
}

TEST(ModelTest, HitsWallAndCollide) {
  GameModel m;
  m.ApplyAction(Start, false);
  m.Tick();
  m.ApplyAction(Up, false);
  for (int i = 0; i < 20; ++i) m.Tick();
  EXPECT_EQ(m.state(), GameState::GameOver);
}

TEST(ModelTest, CollideGoesToGameOver) {
  GameModel m;
  m.ApplyAction(Start, false);
  m.Tick();
  m.ApplyAction(Up, false);
  for (int i = 0; i < 20; ++i) {
    m.Tick();
    if (m.state() == GameState::Collide) break;
  }
  EXPECT_EQ(m.state(), GameState::Collide);
  m.Tick();
  EXPECT_EQ(m.state(), GameState::GameOver);
}

TEST(ModelTest, PauseFreezesSnake) {
  GameModel m;
  m.ApplyAction(Start, false);
  m.Tick();
  m.ApplyAction(Pause, false);
  for (int i = 0; i < 100; ++i) m.Tick();
  EXPECT_EQ(m.state(), GameState::Paused);
}

TEST(ModelTest, RestartAfterGameOver) {
  GameModel m;
  m.ApplyAction(Start, false);
  m.Tick();
  m.ApplyAction(Terminate, false);
  EXPECT_EQ(m.state(), GameState::GameOver);
  m.ApplyAction(Start, false);
  EXPECT_EQ(m.state(), GameState::Start);
}

TEST(ModelTest, FoodIncreasesScore) {
  GameModel m;
  GameInfo_t info = {};
  int field[20][10] = {};
  int next[4][4] = {};
  int* fp[20];
  int* np[4];
  for (int i = 0; i < 20; ++i) fp[i] = field[i];
  for (int i = 0; i < 4; ++i) np[i] = next[i];
  info.field = fp;
  info.next = np;

  m.ApplyAction(Start, false);
  m.Tick();
  m.FillGameInfo(&info);
  EXPECT_EQ(info.score, 0);
  EXPECT_EQ(info.pause, 0);
}

TEST(ModelTest, FieldHasSnakeAfterStart) {
  GameModel m;
  GameInfo_t info = {};
  int field[20][10] = {};
  int next[4][4] = {};
  int* fp[20];
  int* np[4];
  for (int i = 0; i < 20; ++i) fp[i] = field[i];
  for (int i = 0; i < 4; ++i) np[i] = next[i];
  info.field = fp;
  info.next = np;

  m.ApplyAction(Start, false);
  m.Tick();
  m.FillGameInfo(&info);

  int filled = 0;
  for (int y = 0; y < 20; ++y)
    for (int x = 0; x < 10; ++x)
      if (info.field[y][x] != 0) ++filled;

  EXPECT_GE(filled, 5);
}

TEST(ModelTest, FieldEmptyInStart) {
  GameModel m;
  GameInfo_t info = {};
  int field[20][10] = {};
  int next[4][4] = {};
  int* fp[20];
  int* np[4];
  for (int i = 0; i < 20; ++i) fp[i] = field[i];
  for (int i = 0; i < 4; ++i) np[i] = next[i];
  info.field = fp;
  info.next = np;

  m.FillGameInfo(&info);
  int filled = 0;
  for (int y = 0; y < 20; ++y)
    for (int x = 0; x < 10; ++x)
      if (info.field[y][x] != 0) ++filled;
  EXPECT_EQ(filled, 0);
}

TEST(ModelTest, PauseFlag) {
  GameModel m;
  GameInfo_t info = {};
  int field[20][10] = {};
  int next[4][4] = {};
  int* fp[20];
  int* np[4];
  for (int i = 0; i < 20; ++i) fp[i] = field[i];
  for (int i = 0; i < 4; ++i) np[i] = next[i];
  info.field = fp;
  info.next = np;

  m.ApplyAction(Start, false);
  m.Tick();
  m.ApplyAction(Pause, false);
  m.FillGameInfo(&info);
  EXPECT_EQ(info.pause, 1);

  m.ApplyAction(Pause, false);
  m.FillGameInfo(&info);
  EXPECT_EQ(info.pause, 0);
}

TEST(ModelTest, SpeedForLevel) {
  EXPECT_EQ(GameModel::SpeedForLevel(1), 350);
  EXPECT_EQ(GameModel::SpeedForLevel(10), 60);

  int prev = GameModel::SpeedForLevel(1);
  for (int lvl = 2; lvl <= 10; ++lvl) {
    int cur = GameModel::SpeedForLevel(lvl);
    EXPECT_LT(cur, prev);
    prev = cur;
  }

  EXPECT_EQ(GameModel::SpeedForLevel(0), GameModel::SpeedForLevel(1));
  EXPECT_EQ(GameModel::SpeedForLevel(-3), GameModel::SpeedForLevel(1));
  EXPECT_EQ(GameModel::SpeedForLevel(11), GameModel::SpeedForLevel(10));
  EXPECT_EQ(GameModel::SpeedForLevel(100), GameModel::SpeedForLevel(10));
}
TEST(LevelForScoreTest, LevelCapsAtTen) {
  EXPECT_EQ(GameModel::LevelForScore(0), 1);
  EXPECT_EQ(GameModel::LevelForScore(2), 1);
  EXPECT_EQ(GameModel::LevelForScore(6), 2);
  EXPECT_EQ(GameModel::LevelForScore(7), 2);
  EXPECT_EQ(GameModel::LevelForScore(50), 10);
  EXPECT_EQ(GameModel::LevelForScore(100), 10);
  EXPECT_EQ(GameModel::LevelForScore(199), 10);
  EXPECT_EQ(GameModel::LevelForScore(200), 10);
}

TEST(ModelTest, ChangeDirectionDown) {
  GameModel m;
  m.ApplyAction(Start, false);
  m.Tick();
  m.ApplyAction(Down, false);
  m.Tick();
  EXPECT_EQ(m.state(), GameState::Moving);
}

TEST(ModelTest, ChangeDirectionLeft) {
  GameModel m;
  m.ApplyAction(Start, false);
  m.Tick();
  m.ApplyAction(Up, false);
  m.Tick();
  m.ApplyAction(Left, false);
  m.Tick();
  EXPECT_EQ(m.state(), GameState::Moving);
}

TEST(ModelTest, OppositeDirectionIgnored) {
  GameModel m;
  m.ApplyAction(Start, false);
  m.Tick();
  m.ApplyAction(Left, false);
  m.Tick();
  EXPECT_EQ(m.state(), GameState::Moving);
  EXPECT_EQ(m.score(), 0);
}

TEST(ModelTest, DirectionChangeInPausedState) {
  GameModel m;
  m.ApplyAction(Start, false);
  m.Tick();
  m.ApplyAction(Pause, false);

  m.ApplyAction(Up, false);
  m.ApplyAction(Pause, false);
  m.Tick();

  EXPECT_EQ(m.state(), GameState::Moving);
}

TEST(ModelTest, ChangeDirectionRight) {
  GameModel m;
  m.ApplyAction(Start, false);
  m.Tick();
  m.ApplyAction(Up, false);
  m.Tick();
  m.ApplyAction(Right, false);
  m.Tick();
  EXPECT_EQ(m.state(), GameState::Moving);
}
TEST(ModelTest, EatingFoodUpdatesAllCounters) {
  std::remove("snake_record.txt");

  GameModel m;
  m.ApplyAction(Start, false);
  m.Tick();
  m.SetFood({6, 10});
  m.Tick();

  EXPECT_EQ(m.score(), 1);

  GameInfo_t info = {};
  int field[20][10] = {};
  int next[4][4] = {};
  int* fp[20];
  int* np[4];
  for (int i = 0; i < 20; ++i) fp[i] = field[i];
  for (int i = 0; i < 4; ++i) np[i] = next[i];
  info.field = fp;
  info.next = np;
  m.FillGameInfo(&info);

  EXPECT_EQ(info.score, 1);
  EXPECT_EQ(info.high_score, 1);
  EXPECT_EQ(info.level, 1);

  int filled = 0;
  for (int y = 0; y < 20; ++y)
    for (int x = 0; x < 10; ++x)
      if (info.field[y][x] != 0) ++filled;
  EXPECT_EQ(filled, 5);

  m.Tick();
  m.FillGameInfo(&info);
  filled = 0;
  for (int y = 0; y < 20; ++y)
    for (int x = 0; x < 10; ++x)
      if (info.field[y][x] != 0) ++filled;
  EXPECT_EQ(filled, 6);
}

TEST(ModelTest, ReachingWinLength) {
  GameModel m(5);            
  m.ApplyAction(Start, false);      
  m.Tick();                      

  m.SetFood({6, 10});              
  m.Tick();                        

  EXPECT_EQ(m.state(), GameState::GameOver);
  EXPECT_EQ(m.score(), 1);
}

TEST(ModelTest, NoWinBelowWinLength) {
  GameModel m(10);                
  m.ApplyAction(Start, false);
  m.Tick();

  m.SetFood({6, 10});             
  m.Tick();

  EXPECT_EQ(m.state(), GameState::Moving);
  EXPECT_EQ(m.score(), 1);
}
