#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "../../brick_game/brick_game.h"

class Controller {
 private:
  GameType game_;
  bool init_ = false;

 public:
  explicit Controller(GameType game);
  ~Controller();
  void HandleAction(UserAction_t action);
  GameInfo_t Tick();
};
#endif