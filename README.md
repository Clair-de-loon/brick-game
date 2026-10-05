# BrickGame

A modular, multi-frontend implementation of the classic BrickGame, featuring two complete games: **Tetris** and **Snake**. The project demonstrates a clean separation between game logic backends and user interface frontends, making the same game engines reusable across different presentation layers.

## Architecture

- **Two independent backends**: Snake is implemented in C++20 using a finite state machine and the MVC pattern. Tetris is written in C11.
- **Strict separation of concerns**: No business logic in the view code, no interface code in the model. Controllers are thin and only translate input into model commands.
- **Reusable game logic**: Both the Snake and Tetris backends are completely decoupled from the frontends.
- **Multiple frontends**: A console interface based on ncurses and a desktop interface based on Qt. Both support both games.
- **Finite State Machine**: The Snake game logic is formalized as an FSM (Start, Spawn, Moving, Shifting, Pause, GameOver, Win).
- **Modern C++**: Snake uses C++20, RAII, Google Style, and the `s21` namespace.
- **Testing**: The Snake library is covered by unit tests using GTest, with at least 80% code coverage. FSM states and transitions are explicitly tested.
- **Build system**: A Makefile provides the standard GNU targets: `all`, `install`, `uninstall`, `clean`, `dvi`, `dist`, `test`.

## Build

text
```
make all
```

`make all` produces two binaries:

- `build/s21_brick_game` — console frontend (ncurses)
- `build/qt_s21_brick_game` — desktop frontend (Qt)

Other targets: `install`, `uninstall`, `clean`, `dvi`, `dist`, `test`.

## Testing

text
```
make test
```

Unit tests use GTest. The Snake library is tested for FSM states, transitions, and game logic, with coverage of at least 80%.

## Tutorial: Controls

### Tetris

| Key | Action |
| --- | --- |
| Arrow Left / Arrow Right | Move tetromino left or right |
| Arrow Down | Drop tetromino faster |
| Spacebar | Rotate tetromino |
| Enter | Start the game from the main menu |
| P | Pause or resume the game |
| Esc | Terminate or exit the game |

### Snake

| Key | Action |
| --- | --- |
| Arrow Left / Arrow Right / Arrow Up / Arrow Down | Change snake direction |
| Spacebar | Accelerate snake movement |
| Enter | Start the game from the main menu |
| P | Pause or resume the game |
| Esc | Terminate or exit the game |

The snake can only turn left or right relative to its current direction of movement.

## Snake Game Mechanics

- Playing field: 10 cells wide, 20 cells high.
- Initial snake length: 4 cells.
- The snake moves forward automatically on the game timer.
- Eating an apple increases length by 1 and adds 1 point.
- When length reaches 200, the game ends and the player wins.
- Hitting a boundary or itself ends the game with a loss.
- Action key speeds up the snake.

## Tetris Game Mechanics

- Playing field: 10 columns by 20 rows.
- Seven standard tetromino shapes: I, O, T, S, Z, J, L.
- Tetrominoes fall one row at a time according to the game timer.
- Player can move left/right, rotate, and accelerate descent.
- When a tetromino lands, it becomes part of the field.
- Completed horizontal lines are cleared, and rows above shift down.
- Clearing multiple lines awards more points.
- Score increases with each line cleared.
- Level increases as score grows; each level increases falling speed.
- Game ends when a new tetromino cannot be placed at the top.
- The Tetris logic library uses a finite state machine (Start, Spawn, Moving, Shifting, Pause, GameOver).
- The library provides functions to handle user input and to output the current field matrix whenever it changes.
