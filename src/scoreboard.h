#pragma once

#include <QMap>
#include <QTableWidget>

class ScoreTracker {
public:
    void recordResult(const QString &winner);
    QMap<QString, int> scores() const { return m_scores; }

private:
    QMap<QString, int> m_scores;
};

class ScoreBoard : public QTableWidget {
    Q_OBJECT
public:
    explicit ScoreBoard(QWidget *parent = nullptr);

    void refresh(const ScoreTracker &tracker);
};
