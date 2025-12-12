#include "scoreboard.h"

#include <QAbstractItemView>
#include <QHeaderView>

void ScoreTracker::recordResult(const QString &winner) {
    m_scores[winner] += 1;
}

ScoreBoard::ScoreBoard(QWidget *parent) : QTableWidget(parent) {
    setColumnCount(2);
    setHorizontalHeaderLabels({tr("Side"), tr("Score")});
    horizontalHeader()->setStretchLastSection(true);
    verticalHeader()->hide();
    setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void ScoreBoard::refresh(const ScoreTracker &tracker) {
    const auto data = tracker.scores();
    setRowCount(data.size());
    int row = 0;
    for (auto it = data.begin(); it != data.end(); ++it, ++row) {
        setItem(row, 0, new QTableWidgetItem(it.key()));
        setItem(row, 1, new QTableWidgetItem(QString::number(it.value())));
    }
}
