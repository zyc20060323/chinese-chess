#pragma once

#include "boardstate.h"
#include "scoreboard.h"

#include <QObject>
#include <QString>
#include <memory>

class AIEngine;
class NetworkManager;

class GameController : public QObject {
    Q_OBJECT
public:
    enum class Mode { HumanVsHuman, HumanVsAI, NetworkHost, NetworkClient };

    explicit GameController(QObject *parent = nullptr);

    void startNewGame(Mode mode, const QString &host = {}, quint16 port = 45678);

    bool selectSquare(const QPoint &coord);
    bool moveSelectedTo(const QPoint &coord);
    bool undoLastMove();

    void offerDraw();
    void acceptDraw();

    const BoardState &boardState() const { return *m_board; }
    const ScoreTracker &scores() const { return m_scores; }

signals:
    void boardChanged();
    void evaluationUpdated(double redWin, double blackWin);
    void scoreChanged();

private:
    void applyMove(const Move &move);
    void onNetworkMoveReceived(const Move &move);
    void onNetworkDrawOffered();
    void onNetworkDrawAccepted();
    void evaluatePosition();
    void pushScore(const QString &winner);

    std::unique_ptr<BoardState> m_board;
    std::unique_ptr<AIEngine> m_ai;
    std::unique_ptr<NetworkManager> m_network;
    ScoreTracker m_scores;
    Mode m_mode{Mode::HumanVsHuman};
    std::optional<QPoint> m_selected;
    bool m_waitingForNetwork{false};
};
