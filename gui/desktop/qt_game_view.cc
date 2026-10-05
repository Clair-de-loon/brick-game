#include "qt_game_view.h"

#include <QKeyEvent>
#include <QPainter>

#include "gui/desktop/qt_controller.h"
#include "qcolor.h"
#include "qglobal.h"
#include "qnamespace.h"
#include "qwidget.h"
#include "qwindowdefs.h"

GameView::GameView(Controller* contr, QWidget* parent)
    : QWidget(parent), contr_(contr) {
  setFixedSize(FieldPixW + PanelWidth, FieldPixH);
  setFocusPolicy(Qt::StrongFocus);  // get keyboard

  timer_ = new QTimer(this);
  connect(timer_, &QTimer::timeout, this, &GameView::OnTimerTick);
  // start tick

  info_ = contr_->Tick();
  timer_->start(info_.speed);
}

void GameView::OnTimerTick() {
  info_ = contr_->Tick();

  bool has_any_cell = false;
  for (int y = 0; y < Height && !has_any_cell; ++y) {
    for (int x = 0; x < Width; ++x) {
      if (info_.field[y][x] != 0) {
        has_any_cell = true;
        break;
      }
    }
  }
  // to draw you win or game over
  if (has_any_cell) {
    game_start_ = true;
    game_over_ = false;
  } else if (game_start_) {
    game_over_ = true;
  }
  update();  // qt redraw
  timer_->start(info_.speed);
}
void GameView::keyPressEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key_Escape) {
    close();
    return;
  }
  UserAction_t act;
  switch (event->key()) {
    case Qt::Key_Up:
      act = Up;
      break;
    case Qt::Key_Down:
      act = Down;
      break;
    case Qt::Key_Left:
      act = Left;
      break;
    case Qt::Key_Right:
      act = Right;
      break;
    case Qt::Key_P:
      act = Pause;
      break;
    case Qt::Key_Space:
      act = Action;
      break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
      act = Start;
      break;
    default:
      return;
  }
  contr_->HandleAction(act);
  update();
}
void GameView::paintEvent(QPaintEvent* event) {
  Q_UNUSED(event);
  QPainter painter(this);
  painter.fillRect(rect(), QColor(30, 30, 30));

  DrawField(painter);
  DrawPanel(painter);
  DrawOverlay(painter);
}

void GameView::DrawField(QPainter& painter) {
  if (!info_.field) return;

  for (int y = 0; y < Height; ++y) {
    for (int x = 0; x < Width; ++x) {
      QRect cell(x * CellSize, y * CellSize, CellSize, CellSize);

      QColor color;

      switch (info_.field[y][x]) {
        case 0:
          color = QColor(24, 28, 36);  // backround
          break;
        case 1:
          color = QColor(241, 196, 15);  // orange snake
          break;
        case 2:
          color = QColor(155, 89, 182);  // purple
          break;
        case 3:
          color = QColor(52, 152, 219);  // blue
          break;
        case 4:
          color = QColor(46, 204, 113);  // green
          break;
        case 5:
          color = QColor(230, 126, 34);  // light orange snake head
          break;
        case 6:
          color = QColor(231, 76, 60);  // red food
          break;
        case 7:
          color = QColor(244, 143, 177);  // pink
          break;
        default:
          color = QColor(26, 188, 156);
          break;
      }
      painter.fillRect(cell, color);

      // draw bounds
      painter.setPen(QColor(60, 60, 60));
      painter.drawRect(cell);
    }
  }
}

void GameView::DrawPanel(QPainter& painter) {
  QFont font = painter.font();
  font.setPointSize(14);
  font.setBold(true);
  painter.setFont(font);
  painter.setPen(Qt::white);

  const int x = PanelX;
  const int line = 30;
  int y = 50;

  painter.drawText(x, y, "RECORD:");
  painter.drawText(x, y + line, QString::number(info_.high_score));

  y += 3 * line;
  painter.drawText(x, y, "SCORE:");
  painter.drawText(x, y + line, QString::number(info_.score));
}

void GameView::DrawOverlay(QPainter& painter) {
  QString text;
  if (!game_start_) {
    text = "Press ENTER to play";
  } else if (game_over_) {
    text = "Game Over. Press ENTER";
  } else if (info_.pause) {
    text = "PAUSED. Press P";
  }

  if (text.isEmpty()) return;

  QFont font = painter.font();
  font.setPointSize(16);
  font.setBold(true);
  painter.setFont(font);

  QRect band(0, FieldPixH / 2 - 30, FieldPixW, 60);
  painter.fillRect(band, QColor(15, 18, 24, 200));

  painter.setPen(Qt::white);
  painter.drawText(band, Qt::AlignCenter, text);
}