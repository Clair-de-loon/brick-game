CC     := gcc
CXX    := g++
CFLAGS := -std=c11   -Wall -Wextra -Werror -I. -g
CXXFLAGS := -std=c++20 -Wall -Wextra -Werror -I. -g
LDLIBS      := -lncurses -lpthread -lm
CHECK_LIBS  := -lcheck -lsubunit -lrt -lm -lpthread
GTEST_LIBS  := -lgtest -lgtest_main -pthread

QT_CFLAGS := $(shell pkg-config --cflags Qt6Widgets)
QT_LIBS   := $(shell pkg-config --libs Qt6Widgets)

OS := $(shell uname -s)
ifeq ($(OS),Linux)
  CHECK_LIBS += -lsubunit
endif
BUILD := build
#bin
BIN         := $(BUILD)/s21_brick_game
TETRIS_TEST := $(BUILD)/test_tetris
SNAKE_TEST  := $(BUILD)/test_snake
TETRIS_LIB  := $(BUILD)/libtetris.a
SNAKE_LIB   := $(BUILD)/libsnake.a
QT_BIN  := $(BUILD)/qt_s21_brick_game
#src
TETRIS_SRCS := $(wildcard brick_game/tetris/*.c) \
               brick_game/brick_game.c \
               brick_game/dispatcher.c
SNAKE_SRCS  := $(wildcard brick_game/snake/*.cc)
CLI_SRCS    := $(wildcard gui/cli/*.c)
TEST_T_SRCS := $(wildcard tests/test_tetris.c)
TEST_S_SRCS := $(wildcard tests/test_snake.cc) $(wildcard tests/test_snake.cpp)
QT_SRCS := $(wildcard gui/desktop/*.cc)
#obj
TETRIS_OBJS := $(patsubst %.c,   $(BUILD)/%.o, $(TETRIS_SRCS))
SNAKE_OBJS  := $(patsubst %.cc,  $(BUILD)/%.o, $(SNAKE_SRCS))
CLI_OBJS    := $(patsubst %.c,   $(BUILD)/%.o, $(CLI_SRCS))
TEST_T_OBJS := $(patsubst %.c,   $(BUILD)/%.o, $(TEST_T_SRCS))
TEST_S_OBJS := $(patsubst %.cc,  $(BUILD)/%.o, $(TEST_S_SRCS)) \
               $(patsubst %.cpp, $(BUILD)/%.o, $(filter %.cpp,$(TEST_S_SRCS)))
QT_OBJS := $(patsubst gui/desktop/%.cc, $(BUILD)/gui/desktop/%.o, $(QT_SRCS))



FMT_FILES := $(shell find brick_game gui tests -type f \
              \( -name '*.c' -o -name '*.h' -o -name '*.cc' -o -name '*.cpp' -o -name '*.hpp' \) 2>/dev/null)

.PHONY: all clean rebuild test test_tetris test_snake gcov_report open \
        install uninstall dvi dist format style valgrind

all: $(BIN) $(QT_BIN)

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: %.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -Ibrick_game/snake/include -c $< -o $@

$(BUILD)/%.o: %.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -Ibrick_game/snake/include -c $< -o $@

$(BUILD)/gui/desktop/%.o: gui/desktop/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(QT_CFLAGS) -c $< -o $@

$(TETRIS_LIB): $(TETRIS_OBJS)
	@mkdir -p $(dir $@)
	ar rcs $@ $^

$(SNAKE_LIB): $(SNAKE_OBJS)
	@mkdir -p $(dir $@)
	ar rcs $@ $^

$(QT_BIN): $(QT_OBJS) $(TETRIS_LIB) $(SNAKE_LIB) $(TETRIS_LIB)
	$(CXX) $(QT_OBJS) $(TETRIS_LIB) $(SNAKE_LIB) $(TETRIS_LIB) -o $@ $(QT_LIBS) $(LDLIBS)

$(BIN): $(CLI_OBJS) $(TETRIS_LIB) $(SNAKE_LIB)
	@mkdir -p $(dir $@)
	$(CXX) $(CLI_OBJS) $(TETRIS_LIB) $(SNAKE_LIB) -o $@ $(LDLIBS)

$(TETRIS_TEST): $(TEST_T_OBJS) $(TETRIS_LIB)
	@mkdir -p $(dir $@)
	$(CC) $^ -o $@ $(CHECK_LIBS) $(LDLIBS)

$(SNAKE_TEST): $(TEST_S_OBJS) $(SNAKE_LIB)
	@mkdir -p $(dir $@)
	$(CXX) $^ -o $@ $(GTEST_LIBS)

test_tetris: $(TETRIS_TEST)
	./$(TETRIS_TEST)

test_snake: $(SNAKE_TEST)
	./$(SNAKE_TEST)

test: test_tetris test_snake

gcov_report: CFLAGS += --coverage
gcov_report: CXXFLAGS += --coverage
gcov_report: LDLIBS += --coverage
gcov_report: CHECK_LIBS += --coverage
gcov_report: GTEST_LIBS += --coverage
gcov_report: clean test
	@mkdir -p report
	gcovr -r . --html --html-details -o report/index.html \
		--exclude '.*tests.*' \
		--exclude '.*gui.*' \
		--exclude '.*dispatcher\.c' \
		--exclude '.*snake_api\.cc' \
		--exclude '.*brick_game\.c' \
		--exclude '.*\.h' \
		--exclude '.*main.*'
	@echo "report/index.html"

open: gcov_report
	xdg-open report/index.html

PREFIX ?= $(HOME)/.local

install: all
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(BIN) $(DESTDIR)$(PREFIX)/bin/

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(notdir $(BIN))

clean:
	rm -rf $(BUILD) report doc brick_game_dist brick_game.tar.gz

rebuild: clean all

dvi:
	@mkdir -p doc
	@cp README.md doc/documentation.md

dist: clean
	@mkdir -p brick_game_dist
	@cp -r brick_game gui tests Makefile README.md brick_game_dist/
	tar -czf brick_game.tar.gz brick_game_dist/
	@rm -rf brick_game_dist

clang:
	clang-format -i $(FMT_FILES)

style:
	clang-format -n --Werror $(FMT_FILES)

valgrind: test
	valgrind --tool=memcheck --leak-check=full \
		--suppressions=ncurses.supp ./$(SNAKE_TEST)