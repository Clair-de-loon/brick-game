#include "../../brick_game/brick_game.h"
#include "ncurses.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#define WIN_INIT           \
  {                        \
    initscr();             \
    noecho();              \
    curs_set(0);           \
    keypad(stdscr, TRUE);  \
    nodelay(stdscr, TRUE); \
  }
#define MVADDCH(y, x, c) mvaddch((2 + y), (2 + x), c)

static void initOverlay();  // ui, box
static void printOverlay(int **field, int **next, int score,
                         int record);  // updating
static void printRectangle(int top_y, int bottom_y, int left_x, int right_x);
static void printPause();
static int gameLoop();
static int procInput(int signal);
static void initColors();
static int selectGameMenu(GameType *out);
static void printField(int **field);  // debug

int main() {
  WIN_INIT;
  start_color();
  bkgd(COLOR_WHITE);
  initColors();
  srand(time(NULL));

  GameType game;
  while (selectGameMenu(&game)) {
    selectGame(game);
    if (gameLoop()) {
      endwin();
      return 1;
    }
  }
  if (gameLoop()) {
    return 1;
  }
  endwin();
  return 0;
}

static int gameLoop() {
  int status = 0;
  bool loop = 1;
  bool hold = 1;
  int signal = 0;
  if (hold) {
    hold = 0;
  }

  initOverlay();
  int initState = initGameInfo();

  if (initState == 1) {  // calloc fail check
    status = 1;
    loop = 0;
  }
  refresh();

  mvprintw(18, 26, "Press ENTER to play");

  while (loop) {
    const GameInfo_t info = updateCurrentState();
    if (info.pause) {
      timeout(info.speed);
      printPause();
    } else {
      mvprintw(20, 26, "                  ");
      timeout(info.speed);
    }
    signal = wgetch(stdscr);  // input
    loop = procInput(signal);
    if (loop) {
      printOverlay(info.field, info.next, info.score, info.high_score);
      printField(info.field);  // debug
      refresh();
    }
  }
  freeGameInfo(getGameInfoPtr());

  return status;
}
static int selectGameMenu(GameType *out) {
  const char *names[2] = {"Tetris", "Snake"};
  const int count = 2;
  int choice = 0;
  int ch;

  timeout(-1);
  keypad(stdscr, TRUE);

  while (1) {
    attrset(A_NORMAL);
    clear();
    refresh();
    for (int i = 0; i < count; ++i) {
      if (i == choice) attron(A_REVERSE);
      mvprintw(8 + i * 2, 12, "  %-8s  ", names[i]);
      if (i == choice) attroff(A_REVERSE);
    }
    refresh();

    ch = wgetch(stdscr);
    if (ch == KEY_UP && choice > 0)
      --choice;
    else if (ch == KEY_DOWN && choice < count - 1)
      ++choice;
    else if (ch == '\n' || ch == '\r' || ch == KEY_ENTER) {
      *out = (choice == 0) ? kTetris : kSnake;
      return 1;
    } else if (ch == 27) {
      return 0;
    }
  }
}
static int procInput(int signal) {
  int res = 1;
  UserAction_t action = getAction(signal);
  if (action == Terminate) {
    res = 0;
  }
  if (action == Start) {  // clear "press enter"
    mvprintw(18, 26, "                   ");
  }

  if (res != 0) {
    userInput(action, 0);
  }

  return res;
}

static void initColors() {
  init_pair(1, COLOR_GREEN, COLOR_GREEN);
  init_pair(2, COLOR_GREEN, COLOR_MAGENTA);
  init_pair(3, COLOR_WHITE, COLOR_WHITE);
  init_pair(4, COLOR_WHITE, COLOR_BLUE);
  init_pair(5, COLOR_WHITE, COLOR_CYAN);
  init_pair(6, COLOR_WHITE, COLOR_RED);
  init_pair(7, COLOR_WHITE, COLOR_YELLOW);
  init_pair(8, COLOR_GREEN, COLOR_BLUE);
}

static void initOverlay() {
  printRectangle(0, 21, 0, 21);  // playfield 10x20
  bkgd(COLOR_PAIR(0));
  mvprintw(4, 26, "RECORD:");
  mvprintw(8, 26, "SCORE:");
}
static void printPause() {
  attron(COLOR_PAIR(6));
  mvprintw(20, 26, "Press P to unpause");
  attroff(COLOR_PAIR(6));
}
static void printBlock(int x, int y, int type) {
  attron(COLOR_PAIR(type));
  MVADDCH(y, x * 2 + 1, ' ');
  MVADDCH(y, x * 2 + 2, ' ');
  attroff(COLOR_PAIR(type));
}

static void printOverlay(int **field, int **next, int score, int record) {
  mvprintw(6, 26, "         ");
  mvprintw(10, 26, "         ");
  mvprintw(6, 26, "%d", record);
  mvprintw(10, 26, "%d", score);

  // field data
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10; j++) {
      printBlock(j, i + 1, field[i][j]);
    }
  }
  // next data
  for (int k = 0; k < 4; k++) {
    for (int r = 0; r < 4; r++) {
      printBlock(k + 12, r + 10, next[r][k]);
      // mvprintw( 12, 26, "%d//////", next[r][k]);
    }
  }
}

static void printRectangle(int top_y, int bottom_y, int left_x, int right_x) {
  MVADDCH(top_y, left_x, ACS_ULCORNER);
  MVADDCH(top_y, right_x, ACS_URCORNER);
  MVADDCH(bottom_y, left_x, ACS_LLCORNER);
  MVADDCH(bottom_y, right_x, ACS_LRCORNER);

  for (int x = left_x + 1; x < right_x; x++) {
    MVADDCH(top_y, x, ACS_HLINE);
    MVADDCH(bottom_y, x, ACS_HLINE);
  }

  for (int y = top_y + 1; y < bottom_y; y++) {
    MVADDCH(y, left_x, ACS_VLINE);
    MVADDCH(y, right_x, ACS_VLINE);
  }
}

static void printField(int **field) {  // debug !!!
  FILE *file = fopen("debug.txt", "w");
  if (file) {
    for (int i = 0; i < 20; i++) {
      fputs("\n", file);
      for (int j = 0; j < 10; j++) {
        fprintf(file, "%d ", field[i][j]);
      }
    }
    fclose(file);
  }
}
