#include "boardstate.h"

#include <algorithm>
#include <cmath>

namespace {
std::vector<PieceSnapshot> defaultPieces() {
    return {
        {{0, 9}, "車", true}, {{1, 9}, "馬", true}, {{2, 9}, "相", true}, {{3, 9}, "仕", true},
        {{4, 9}, "帥", true}, {{5, 9}, "仕", true}, {{6, 9}, "相", true}, {{7, 9}, "馬", true}, {{8, 9}, "車", true},
        {{1, 7}, "炮", true}, {{7, 7}, "炮", true}, {{0, 6}, "兵", true}, {{2, 6}, "兵", true},
        {{4, 6}, "兵", true}, {{6, 6}, "兵", true}, {{8, 6}, "兵", true},
        {{0, 0}, "車", false}, {{1, 0}, "馬", false}, {{2, 0}, "象", false}, {{3, 0}, "士", false},
        {{4, 0}, "将", false}, {{5, 0}, "士", false}, {{6, 0}, "象", false}, {{7, 0}, "馬", false}, {{8, 0}, "車", false},
        {{1, 2}, "砲", false}, {{7, 2}, "砲", false}, {{0, 3}, "卒", false}, {{2, 3}, "卒", false},
        {{4, 3}, "卒", false}, {{6, 3}, "卒", false}, {{8, 3}, "卒", false},
    };
}
}

BoardState::BoardState() : m_pieces(defaultPieces()) {}

std::optional<PieceSnapshot> BoardState::pieceAt(const QPoint &coord) const {
    const auto it = std::find_if(m_pieces.begin(), m_pieces.end(), [&](const PieceSnapshot &p) { return p.position == coord; });
    if (it == m_pieces.end()) {
        return std::nullopt;
    }
    return *it;
}

std::optional<Move> BoardState::createMove(const QPoint &from, const QPoint &to) const {
    const auto piece = pieceAt(from);
    if (!piece.has_value()) {
        return std::nullopt;
    }
    if (piece->isRed != m_redToMove) {
        return std::nullopt;
    }
    if (!validateMove(*piece, to)) {
        return std::nullopt;
    }

    Move move{from, to, "", false};
    const auto captured = pieceAt(to);
    if (captured.has_value()) {
        move.capturedLabel = captured->label;
        move.capturedRed = captured->isRed;
    }
    return move;
}

void BoardState::applyMove(const Move &move) {
    auto it = std::find_if(m_pieces.begin(), m_pieces.end(), [&](const PieceSnapshot &p) { return p.position == move.to; });
    if (it != m_pieces.end()) {
        m_pieces.erase(it);
    }

    for (auto &piece : m_pieces) {
        if (piece.position == move.from) {
            piece.position = move.to;
            break;
        }
    }

    m_history.push_back(move);
    m_redToMove = !m_redToMove;
}

bool BoardState::undo() {
    if (m_history.empty()) {
        return false;
    }
    const Move last = m_history.back();
    m_history.pop_back();

    for (auto &piece : m_pieces) {
        if (piece.position == last.to) {
            piece.position = last.from;
            break;
        }
    }

    if (!last.capturedLabel.isEmpty()) {
        m_pieces.push_back({last.to, last.capturedLabel, last.capturedRed});
    }

    m_redToMove = !m_redToMove;
    return true;
}

bool BoardState::isInsidePalace(const QPoint &pos, bool red) const {
    const int minRow = red ? 7 : 0;
    const int maxRow = red ? 9 : 2;
    return pos.x() >= 3 && pos.x() <= 5 && pos.y() >= minRow && pos.y() <= maxRow;
}

bool BoardState::validateMove(const PieceSnapshot &piece, const QPoint &to) const {
    if (piece.position == to) {
        return false;
    }

    const auto captured = pieceAt(to);
    if (captured.has_value() && captured->isRed == piece.isRed) {
        return false;
    }

    const QPoint delta = to - piece.position;
    const int dx = delta.x();
    const int dy = delta.y();

    const auto label = piece.label;
    if (label == "兵" || label == "卒") {
        if ((piece.isRed && dy >= 0) || (!piece.isRed && dy <= 0)) {
            return false; // must move forward
        }
        if (std::abs(dx) + std::abs(dy) != 1) {
            if ((piece.isRed && piece.position.y() <= 4) || (!piece.isRed && piece.position.y() >= 5)) {
                if (std::abs(dx) == 1 && dy == 0) {
                    return true;
                }
            }
            return false;
        }
        return true;
    }

    if (label == "車" || label == "车") {
        if (dx != 0 && dy != 0) {
            return false;
        }
        return pathClear(piece.position, to);
    }

    if (label == "馬" || label == "马") {
        if (!((std::abs(dx) == 1 && std::abs(dy) == 2) || (std::abs(dx) == 2 && std::abs(dy) == 1))) {
            return false;
        }
        const QPoint legBlock = piece.position + QPoint(dx == 2 || dx == -2 ? dx / 2 : 0, dy == 2 || dy == -2 ? dy / 2 : 0);
        return !pieceAt(legBlock).has_value();
    }

    if (label == "炮" || label == "砲") {
        if (dx != 0 && dy != 0) {
            return false;
        }
        const int between = countPiecesBetween(piece.position, to);
        if (captured.has_value()) {
            return between == 1;
        }
        return between == 0;
    }

    if (label == "相" || label == "象") {
        if (std::abs(dx) != 2 || std::abs(dy) != 2) {
            return false;
        }
        const QPoint eye(piece.position.x() + dx / 2, piece.position.y() + dy / 2);
        if (pieceAt(eye).has_value()) {
            return false;
        }
        if (piece.isRed && to.y() < 5) return false;
        if (!piece.isRed && to.y() > 4) return false;
        return true;
    }

    if (label == "仕" || label == "士") {
        if (std::abs(dx) != 1 || std::abs(dy) != 1) {
            return false;
        }
        return isInsidePalace(to, piece.isRed);
    }

    if (label == "帥" || label == "将") {
        if (!isInsidePalace(to, piece.isRed)) {
            return false;
        }
        if (std::abs(dx) + std::abs(dy) != 1) {
            return false;
        }
        return true;
    }

    return false;
}

bool BoardState::pathClear(const QPoint &from, const QPoint &to) const {
    if (from.x() == to.x()) {
        const int step = (to.y() > from.y()) ? 1 : -1;
        for (int y = from.y() + step; y != to.y(); y += step) {
            if (pieceAt({from.x(), y}).has_value()) {
                return false;
            }
        }
    } else if (from.y() == to.y()) {
        const int step = (to.x() > from.x()) ? 1 : -1;
        for (int x = from.x() + step; x != to.x(); x += step) {
            if (pieceAt({x, from.y()}).has_value()) {
                return false;
            }
        }
    }
    return true;
}

int BoardState::countPiecesBetween(const QPoint &from, const QPoint &to) const {
    int count = 0;
    if (from.x() == to.x()) {
        const int step = (to.y() > from.y()) ? 1 : -1;
        for (int y = from.y() + step; y != to.y(); y += step) {
            if (pieceAt({from.x(), y}).has_value()) {
                ++count;
            }
        }
    } else if (from.y() == to.y()) {
        const int step = (to.x() > from.x()) ? 1 : -1;
        for (int x = from.x() + step; x != to.x(); x += step) {
            if (pieceAt({x, from.y()}).has_value()) {
                ++count;
            }
        }
    }
    return count;
}

bool BoardState::isCheckmate() const {
    // Simplified placeholder: no legal moves for side to move
    for (const auto &piece : m_pieces) {
        if (piece.isRed != m_redToMove) {
            continue;
        }
        for (int x = 0; x < 9; ++x) {
            for (int y = 0; y < 10; ++y) {
                if (validateMove(piece, {x, y})) {
                    return false;
                }
            }
        }
    }
    return true;
}
