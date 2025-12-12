#pragma once

#include <QMainWindow>
#include <memory>

class GameController;
class ChessBoardWidget;
class ScoreBoard;

class GameWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit GameWindow(QWidget *parent = nullptr);

private slots:
    void onNewGameHumanVsAI();
    void onNewGameHumanVsHuman();
    void onNewGameNetworkHost();
    void onConnectToPeer();
    void onUndoRequested();
    void onOfferDraw();
    void onAcceptDraw();
    void onUpdateEvaluation(double redScore, double blackScore);
    void onUpdateScoreBoard();

private:
    void buildMenus();
    void buildStatusBar();
    void setupConnections();

    std::unique_ptr<GameController> m_controller;
    ChessBoardWidget *m_boardWidget{nullptr};
    ScoreBoard *m_scoreBoard{nullptr};
};
