#include "gamewindow.h"

#include "aiengine.h"
#include "boardstate.h"
#include "chessboardwidget.h"
#include "gamecontroller.h"
#include "networkmanager.h"
#include "scoreboard.h"

#include <QAction>
#include <QDockWidget>
#include <QInputDialog>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>

GameWindow::GameWindow(QWidget *parent) : QMainWindow(parent) {
    m_controller = std::make_unique<GameController>(this);
    m_boardWidget = new ChessBoardWidget(m_controller.get(), this);
    m_scoreBoard = new ScoreBoard(this);

    setCentralWidget(m_boardWidget);

    auto *scoreDock = new QDockWidget(tr("Scoreboard"), this);
    scoreDock->setWidget(m_scoreBoard);
    addDockWidget(Qt::RightDockWidgetArea, scoreDock);

    buildMenus();
    buildStatusBar();
    setupConnections();

    onNewGameHumanVsAI();
}

void GameWindow::buildMenus() {
    auto *gameMenu = menuBar()->addMenu(tr("Game"));
    auto *newHumanVsAI = gameMenu->addAction(tr("New Human vs AI"));
    auto *newHumanVsHuman = gameMenu->addAction(tr("New Local Match"));
    auto *newNetworkHost = gameMenu->addAction(tr("Host LAN Match"));
    auto *connectPeer = gameMenu->addAction(tr("Connect to Host"));
    gameMenu->addSeparator();
    auto *undoAction = gameMenu->addAction(tr("Undo"));
    auto *offerDraw = gameMenu->addAction(tr("Offer Draw"));
    auto *acceptDraw = gameMenu->addAction(tr("Accept Draw"));

    connect(newHumanVsAI, &QAction::triggered, this, &GameWindow::onNewGameHumanVsAI);
    connect(newHumanVsHuman, &QAction::triggered, this, &GameWindow::onNewGameHumanVsHuman);
    connect(newNetworkHost, &QAction::triggered, this, &GameWindow::onNewGameNetworkHost);
    connect(connectPeer, &QAction::triggered, this, &GameWindow::onConnectToPeer);
    connect(undoAction, &QAction::triggered, this, &GameWindow::onUndoRequested);
    connect(offerDraw, &QAction::triggered, this, &GameWindow::onOfferDraw);
    connect(acceptDraw, &QAction::triggered, this, &GameWindow::onAcceptDraw);
}

void GameWindow::buildStatusBar() {
    statusBar()->showMessage(tr("Ready"));
}

void GameWindow::setupConnections() {
    connect(m_controller.get(), &GameController::evaluationUpdated, this, &GameWindow::onUpdateEvaluation);
    connect(m_controller.get(), &GameController::scoreChanged, this, &GameWindow::onUpdateScoreBoard);
    connect(m_boardWidget, &ChessBoardWidget::statusMessage, statusBar(), &QStatusBar::showMessage);
}

void GameWindow::onNewGameHumanVsAI() {
    m_controller->startNewGame(GameController::Mode::HumanVsAI);
    statusBar()->showMessage(tr("Started Human vs AI"));
}

void GameWindow::onNewGameHumanVsHuman() {
    m_controller->startNewGame(GameController::Mode::HumanVsHuman);
    statusBar()->showMessage(tr("Started Human vs Human"));
}

void GameWindow::onNewGameNetworkHost() {
    m_controller->startNewGame(GameController::Mode::NetworkHost);
    statusBar()->showMessage(tr("Hosting LAN match on port 45678"));
}

void GameWindow::onConnectToPeer() {
    bool ok = false;
    const QString peer = QInputDialog::getText(this, tr("Connect to Host"), tr("Hostname:port"), QLineEdit::Normal, "127.0.0.1:45678", &ok);
    if (!ok || peer.isEmpty()) {
        return;
    }

    const auto parts = peer.split(":");
    if (parts.size() != 2) {
        QMessageBox::warning(this, tr("Invalid"), tr("Please enter host:port"));
        return;
    }

    m_controller->startNewGame(GameController::Mode::NetworkClient, parts[0], parts[1].toUInt());
    statusBar()->showMessage(tr("Connecting to %1").arg(peer));
}

void GameWindow::onUndoRequested() {
    if (!m_controller->undoLastMove()) {
        QMessageBox::information(this, tr("Undo"), tr("No move to undo."));
    }
}

void GameWindow::onOfferDraw() {
    m_controller->offerDraw();
}

void GameWindow::onAcceptDraw() {
    m_controller->acceptDraw();
}

void GameWindow::onUpdateEvaluation(double redScore, double blackScore) {
    statusBar()->showMessage(tr("Red win prob: %1% | Black win prob: %2%")
                                 .arg(static_cast<int>(redScore * 100))
                                 .arg(static_cast<int>(blackScore * 100)));
}

void GameWindow::onUpdateScoreBoard() {
    m_scoreBoard->refresh(m_controller->scores());
}
