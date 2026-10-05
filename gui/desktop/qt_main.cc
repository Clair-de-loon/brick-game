#include <QApplication>
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>

#include "qt_controller.h"
#include "qt_game_view.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  QDialog menu;
  menu.setWindowTitle("BrickGame");
  menu.setFixedSize(200, 160);

  auto* layout = new QVBoxLayout(&menu);
  auto* snake = new QPushButton("Snake", &menu);
  auto* tetris = new QPushButton("Tetris", &menu);

  layout->addWidget(snake);
  layout->addWidget(tetris);

  GameType game = kSnake;
  QObject::connect(snake, &QPushButton::clicked, [&]() {
    game = kSnake;
    menu.accept();
  });
  QObject::connect(tetris, &QPushButton::clicked, [&]() {
    game = kTetris;
    menu.accept();
  });

  if (menu.exec() != QDialog::Accepted) return 0;

  Controller controller(game);
  GameView view(&controller);
  view.show();

  return app.exec();
}