#include "chessboardwidget.h"

#include "gamecontroller.h"

#include <QMouseEvent>
#include <QPainter>

namespace {
constexpr int kCellSize = 60;
constexpr int kMargin = 20;
}

ChessBoardWidget::ChessBoardWidget(GameController *controller, QWidget *parent)
    : QWidget(parent), m_controller(controller) {
    setMinimumSize(kMargin * 2 + kCellSize * 8, kMargin * 2 + kCellSize * 9);
}

void ChessBoardWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    for (int r = 0; r < 10; ++r) {
        for (int c = 0; c < 9; ++c) {
            QRect cell(kMargin + c * kCellSize, kMargin + r * kCellSize, kCellSize, kCellSize);
            painter.drawRect(cell);
        }
    }

    const auto pieces = m_controller->boardState().pieces();
    for (const auto &piece : pieces) {
        QRect pieceRect(kMargin + piece.position.x() * kCellSize + 5,
                        kMargin + piece.position.y() * kCellSize + 5,
                        kCellSize - 10, kCellSize - 10);
        painter.setBrush(piece.isRed ? QColor(220, 50, 47) : QColor(38, 139, 210));
        painter.drawEllipse(pieceRect);
        painter.drawText(pieceRect, Qt::AlignCenter, piece.label);
    }

    if (m_selected) {
        painter.setPen(QPen(Qt::darkGreen, 3));
        QRect selectedRect(kMargin + m_selected->x() * kCellSize, kMargin + m_selected->y() * kCellSize, kCellSize, kCellSize);
        painter.drawRect(selectedRect);
    }
}

QPoint ChessBoardWidget::boardCoordinate(const QPoint &pos) const {
    const int c = (pos.x() - kMargin) / kCellSize;
    const int r = (pos.y() - kMargin) / kCellSize;
    return {c, r};
}

void ChessBoardWidget::mousePressEvent(QMouseEvent *event) {
    const QPoint coord = boardCoordinate(event->pos());
    if (coord.x() < 0 || coord.x() > 8 || coord.y() < 0 || coord.y() > 9) {
        return;
    }

    if (!m_selected) {
        if (m_controller->selectSquare(coord)) {
            m_selected = coord;
            update();
            emit statusMessage(tr("Selected %1,%2").arg(coord.x()).arg(coord.y()));
        }
        return;
    }

    if (m_controller->moveSelectedTo(coord)) {
        m_selected.reset();
        update();
    } else {
        emit statusMessage(tr("Illegal move"));
    }
}
