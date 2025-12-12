#pragma once

#include <QWidget>
#include <optional>

class GameController;

class ChessBoardWidget : public QWidget {
    Q_OBJECT
public:
    explicit ChessBoardWidget(GameController *controller, QWidget *parent = nullptr);

signals:
    void statusMessage(const QString &message);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QPoint boardCoordinate(const QPoint &pos) const;

    GameController *m_controller{nullptr};
    std::optional<QPoint> m_selected;
};
