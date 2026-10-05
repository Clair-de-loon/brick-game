#include "include/snake_api.h"

#include "include/model.h"

namespace {

s21::snake::GameModel& GetModel() {
  static s21::snake::GameModel instance;
  return instance;
}

}  // namespace

extern "C" GameInfo_t snake_updateCurrentState(void) {
  s21::snake::GameModel& model = GetModel();
  model.Tick();
  GameInfo_t* info = getGameInfoPtr();
  model.FillGameInfo(info);
  return *info;
}

extern "C" void snake_userInput(UserAction_t action, bool hold) {
  GetModel().ApplyAction(action, hold);
}