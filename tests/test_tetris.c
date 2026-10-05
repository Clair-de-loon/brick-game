#include "../brick_game/tetris/matrix_fsm.h"
#include "../brick_game/tetris/tetris.h"
#include "../brick_game/tetris/tetris_obj.h"
#include "check.h"
#include "stdlib.h"

Params_t *prms;
GameInfo_t *info;
Current_figure *figure;

START_TEST(test_curState) {
  prms->state = MOVING;
  info->pause = 0;

  figure->type = 0;
  figure->x = 4;
  figure->y = 18;
  figure->rotate = 0;
  insertFigure(prms, 0);

  GameInfo_t returned = tetris_updateCurrentState();

  ck_assert_int_eq(prms->state, COLLIDE);

  ck_assert_int_eq(returned.score, info->score);
  ck_assert_ptr_eq(returned.field, info->field);
  ck_assert_ptr_eq(returned.next, info->next);
}
END_TEST

START_TEST(test_userInput_pause) {
  prms->state = MOVING;
  info->pause = 1;
  figure->x = 3;
  figure->y = 5;
  figure->type = 0;

  tetris_userInput(Left, false);
  ck_assert_int_eq(prms->state, MOVING);
  ck_assert_int_eq(figure->x, 3);

  tetris_userInput(Pause, false);

  ck_assert_int_eq(info->pause, 0);
  ck_assert_int_eq(prms->state, MOVING);
}
END_TEST

START_TEST(test_userInput_gameover) {
  prms->state = GAME_OVER;
  for (int i = 0; i < 20; i++)
    for (int j = 0; j < 10; j++) info->field[i][j] = 1;
  info->score = 500;

  tetris_userInput(Start, false);

  ck_assert_int_eq(prms->state, START);
  for (int i = 0; i < 20; i++)
    for (int j = 0; j < 10; j++) ck_assert_int_eq(info->field[i][j], 0);
  ck_assert_int_eq(info->score, 0);
}
END_TEST

START_TEST(test_fsm_moving_to_collide) {
  prms->state = SPAWN;
  spawn(prms);
  ck_assert_int_eq(prms->state, MOVING);

  figure->x = 4;
  figure->y = 18;
  figure->type = 0;  // O-piece
  figure->rotate = 0;

  for (int j = 0; j < 10; j++) info->field[19][j] = 1;

  move_down(prms);
  ck_assert_int_eq(prms->state, COLLIDE);
}
END_TEST

START_TEST(test_updateNextField) {
  figure->next_type = 0;  // O-piece
  updateNextField(prms);
  ck_assert_int_eq(info->next[0][0], 1);
  ck_assert_int_eq(info->next[0][1], 1);
  ck_assert_int_eq(info->next[1][0], 1);
  ck_assert_int_eq(info->next[1][1], 1);
  ck_assert_int_eq(info->next[2][2], 0);
}
END_TEST

START_TEST(test_randomType) {
  for (int i = 0; i < 100; i++) {
    int type = randomType();
    ck_assert(type >= 0 && type < 7);
  }
}
END_TEST

START_TEST(test_eraseLines) {
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10; j++) {
      info->field[i][j] = 0;
    }
  }
  for (int i = 0; i < 10; i++) {
    info->field[19][i] = 2;
    info->field[18][i] = 2;
    info->field[3][i] = 8;
  }
  eraseLines(info);
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10; j++) {
      ck_assert_int_eq(info->field[i][j], 0);
    }
  }
}
END_TEST

START_TEST(test_noEraseLines) {
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10; j++) {
      info->field[i][j] = 0;
    }
  }
  for (int i = 0; i < 10; i++) {
    info->field[19][i] = 2;
    info->field[18][i] = 2;
    info->field[3][i] = 8;
  }
  info->field[19][2] = 0;
  info->field[18][2] = 0;
  info->field[3][2] = 0;
  eraseLines(info);

  for (int i = 0; i < 10; i++) {
    if (i != 2) {
      ck_assert_int_eq(info->field[19][i], 2);
      ck_assert_int_eq(info->field[18][i], 2);
      ck_assert_int_eq(info->field[3][i], 8);
    }
  }
}
END_TEST

START_TEST(test_scoring) {
  int lines = 1;
  scoring(info, lines);
  ck_assert_int_eq(info->score, 100);
  lines = 2;
  scoring(info, lines);
  ck_assert_int_eq(info->score, 400);  //+300
  lines = 4;
  scoring(info, lines);
  ck_assert_int_eq(info->score, 1900);  //+1500
}
END_TEST

// fsm
START_TEST(test_getAct) {
  int input = 0403;
  UserAction_t act = getAction(input);
  ck_assert_int_eq(act, Up);

  input = 0402;
  act = getAction(input);
  ck_assert_int_eq(act, Down);

  input = 0404;
  act = getAction(input);
  ck_assert_int_eq(act, Left);

  input = 0405;
  act = getAction(input);
  ck_assert_int_eq(act, Right);

  input = 27;
  act = getAction(input);
  ck_assert_int_eq(act, Terminate);

  input = 80;
  act = getAction(input);
  ck_assert_int_eq(act, Pause);

  input = 32;
  act = getAction(input);
  ck_assert_int_eq(act, Action);

  input = 10;
  act = getAction(input);
  ck_assert_int_eq(act, Start);
}
END_TEST

START_TEST(test_start) {
  start(prms);
  ck_assert_int_eq(prms->state, 0);
}
END_TEST

START_TEST(test_rotate) {
  int old = prms->cur_figure->rotate;
  rotate(prms);
  ck_assert_int_eq(prms->cur_figure->rotate, old + 1);

  prms->cur_figure->y = 18;
  prms->cur_figure->type = 2;  // T
  old = prms->cur_figure->rotate;
  rotate(prms);
  ck_assert_int_eq(prms->cur_figure->rotate, old);
}
END_TEST

START_TEST(test_moveRight) {
  int oldx = prms->cur_figure->x;
  move_right(prms);
  ck_assert_int_eq(prms->cur_figure->x, oldx + 1);

  prms->cur_figure->x = 9;
  move_right(prms);
  ck_assert_int_eq(prms->cur_figure->x, 9);
}
END_TEST

START_TEST(test_moveLeft) {
  prms->cur_figure->x = 3;
  int oldx = prms->cur_figure->x;
  move_left(prms);
  ck_assert_int_eq(prms->cur_figure->x, oldx - 1);

  prms->cur_figure->x = 0;
  move_left(prms);
  ck_assert_int_eq(prms->cur_figure->x, 0);
}
END_TEST

START_TEST(test_collide) {
  prms->cur_figure->y = 0;
  collide(prms);
  ck_assert_int_eq(prms->state, GAME_OVER);

  prms->cur_figure->y = 8;
  collide(prms);
  ck_assert_int_eq(prms->state, SPAWN);
}
END_TEST

void setup(void) {
  prms = getParamsPtr();
  info = getGameInfoPtr();
  figure = getCurFigurePtr();

  memset(prms, 0, sizeof(Params_t));
  memset(info, 0, sizeof(GameInfo_t));
  memset(figure, 0, sizeof(Current_figure));

  initGameInfo();

  prms->game_info = info;
  prms->cur_figure = figure;
}

void teardown(void) {
  freeGameInfo(info);
  prms->game_info = NULL;
  prms->cur_figure = NULL;
}

Suite *tetris_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Tetris");
  tc_core = tcase_create("Core");

  tcase_add_checked_fixture(tc_core, setup, teardown);

  tcase_add_test(tc_core, test_eraseLines);
  tcase_add_test(tc_core, test_scoring);
  tcase_add_test(tc_core, test_userInput_pause);
  tcase_add_test(tc_core, test_randomType);
  tcase_add_test(tc_core, test_updateNextField);
  tcase_add_test(tc_core, test_fsm_moving_to_collide);
  tcase_add_test(tc_core, test_noEraseLines);
  tcase_add_test(tc_core, test_userInput_gameover);
  tcase_add_test(tc_core, test_curState);
  tcase_add_test(tc_core, test_getAct);
  tcase_add_test(tc_core, test_start);
  tcase_add_test(tc_core, test_rotate);
  tcase_add_test(tc_core, test_moveRight);
  tcase_add_test(tc_core, test_moveLeft);
  tcase_add_test(tc_core, test_collide);
  suite_add_tcase(s, tc_core);
  return s;
}

int main(void) {
  int number_failed;
  Suite *s;
  SRunner *sr;

  s = tetris_suite();
  sr = srunner_create(s);
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);

  srunner_free(sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
