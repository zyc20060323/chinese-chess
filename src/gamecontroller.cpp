#include "gamecontroller.h"

#include "aiengine.h"
#include "networkmanager.h"

GameController::GameController(QObject *parent) : QObject(parent) {
    m_board = std::make_unique<BoardState>();
    m_ai = std::make_unique<AIEngine>();
}

void GameController::startNewGame(Mode mode, const QString &host, quint16 port) {
    m_mode = mode;
    m_board = std::make_unique<BoardState>();
    m_selected.reset();
    m_waitingForNetwork = false;

    if (mode == Mode::NetworkHost) {
        m_network = std::make_unique<NetworkManager>(NetworkManager::Role::Host, this);
        m_network->startListening(port);
        connect(m_network.get(), &NetworkManager::moveReceived, this, &GameController::onNetworkMoveReceived);
        connect(m_network.get(), &NetworkManager::drawOffered, this, &GameController::onNetworkDrawOffered);
        connect(m_network.get(), &NetworkManager::drawAccepted, this, &GameController::onNetworkDrawAccepted);
    } else if (mode == Mode::NetworkClient) {
        m_network = std::make_unique<NetworkManager>(NetworkManager::Role::Client, this);
        connect(m_network.get(), &NetworkManager::moveReceived, this, &GameController::onNetworkMoveReceived);
        connect(m_network.get(), &NetworkManager::drawOffered, this, &GameController::onNetworkDrawOffered);
        connect(m_network.get(), &NetworkManager::drawAccepted, this, &GameController::onNetworkDrawAccepted);
        m_network->connectToHost(host, port);
    } else {
        m_network.reset();
    }

    emit boardChanged();
    emit evaluationUpdated(0.5, 0.5);
}

bool GameController::selectSquare(const QPoint &coord) {
    const auto piece = m_board->pieceAt(coord);
    if (!piece.has_value()) {
        return false;
    }
    if (m_mode == Mode::NetworkClient && piece->isRed) {
        return false;
    }
    if (m_mode == Mode::NetworkHost && !piece->isRed) {
        return false;
    }
    m_selected = coord;
    return true;
}

bool GameController::moveSelectedTo(const QPoint &coord) {
    if (!m_selected) {
        return false;
    }
    const auto move = m_board->createMove(*m_selected, coord);
    if (!move.has_value()) {
        return false;
    }

    applyMove(*move);

    if (m_mode == Mode::HumanVsAI && !m_board->redToMove()) {
        const auto aiMove = m_ai->chooseMove(*m_board);
        if (aiMove.has_value()) {
            applyMove(*aiMove);
        }
    }

    return true;
}

bool GameController::undoLastMove() {
    const bool undone = m_board->undo();
    if (undone) {
        emit boardChanged();
        evaluatePosition();
    }
    return undone;
}

void GameController::offerDraw() {
    if (m_network) {
        m_network->sendDrawOffer();
    }
}

void GameController::acceptDraw() {
    if (m_network) {
        m_network->sendDrawAccepted();
    }
    pushScore("Draw");
}

void GameController::applyMove(const Move &move) {
    m_board->applyMove(move);
    emit boardChanged();

    if (m_network && ((m_mode == Mode::NetworkHost && m_board->redToMove()) || (m_mode == Mode::NetworkClient && !m_board->redToMove()))) {
        m_network->sendMove(move);
        m_waitingForNetwork = true;
    } else {
        m_waitingForNetwork = false;
    }

    if (m_board->isCheckmate()) {
        const QString winner = m_board->redToMove() ? "Black" : "Red";
        pushScore(winner);
    }

    evaluatePosition();
}

void GameController::onNetworkMoveReceived(const Move &move) {
    m_board->applyMove(move);
    m_waitingForNetwork = false;
    emit boardChanged();
    evaluatePosition();
}

void GameController::onNetworkDrawOffered() {
    emit scoreChanged();
}

void GameController::onNetworkDrawAccepted() {
    pushScore("Draw");
}

void GameController::evaluatePosition() {
    const auto score = m_ai->evaluate(*m_board);
    emit evaluationUpdated(score, 1.0 - score);
}

void GameController::pushScore(const QString &winner) {
    m_scores.recordResult(winner);
    emit scoreChanged();
}
