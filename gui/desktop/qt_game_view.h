#ifndef GAME_VIEW_H
#define GAME_VIEW_H

#include <QPaintEvent>
#include <QTimer>
#include <QWidget>

#include "brick_game/brick_game.h"
#include "qt_controller.h"

class GameView : public QWidget {
  // Q_OBJECT
 protected:
  void paintEvent(QPaintEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;

 private:
  void OnTimerTick();
  void DrawField(QPainter& painter);
  void DrawPanel(QPainter& painter);
  void DrawOverlay(QPainter& painter);

  static constexpr int CellSize = 30;
  static constexpr int Width = 10;
  static constexpr int Height = 20;

  static constexpr int PanelWidth = 160;
  static constexpr int FieldPixW = Width * CellSize;
  static constexpr int FieldPixH = Height * CellSize;
  static constexpr int PanelX = FieldPixW + 15;

  Controller* contr_;
  QTimer* timer_;
  GameInfo_t info_{};
  bool game_start_ = 0;
  bool game_over_ = 0;

 public:
  explicit GameView(Controller* contr, QWidget* parent = nullptr);

  // void SetField(int** field);
};

#endif