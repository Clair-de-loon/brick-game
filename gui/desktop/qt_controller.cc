#include "qt_controller.h"

#include "brick_game/brick_game.h"
#include "brick_game/snake/include/types.h"

Controller::Controller(GameType game) : game_(game) {
  selectGame(game_);
  if (initGameInfo() == 0) {
    init_ = true;
  }
}
Controller::~Controller() {
  if (init_) {
    freeGameInfo(getGameInfoPtr());
  }
}
void Controller::HandleAction(UserAction_t action) { userInput(action, 0); }

GameInfo_t Controller::Tick() {
  userInput((UserAction_t)-1, false);
  return updateCurrentState();
}