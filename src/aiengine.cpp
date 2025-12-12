#include "aiengine.h"

#include <QRandomGenerator>

std::optional<Move> AIEngine::chooseMove(const BoardState &state) const {
    std::vector<Move> legalMoves;
    for (const auto &piece : state.pieces()) {
        if (!piece.isRed) {
            continue;
        }
        for (int x = 0; x < 9; ++x) {
            for (int y = 0; y < 10; ++y) {
                const auto move = state.createMove(piece.position, {x, y});
                if (move.has_value()) {
                    legalMoves.push_back(*move);
                }
            }
        }
    }

    if (legalMoves.empty()) {
        return std::nullopt;
    }

    const int index = QRandomGenerator::global()->bounded(static_cast<int>(legalMoves.size()));
    return legalMoves.at(index);
}

double AIEngine::evaluate(const BoardState &state) const {
    // Simplified heuristic: material count
    double redScore = 0.0;
    double blackScore = 0.0;
    for (const auto &piece : state.pieces()) {
        double value = 1.0;
        if (piece.label == "帥" || piece.label == "将") {
            value = 100.0;
        } else if (piece.label == "車" || piece.label == "车") {
            value = 5.0;
        } else if (piece.label == "馬" || piece.label == "马") {
            value = 3.0;
        } else if (piece.label == "炮" || piece.label == "砲") {
            value = 3.0;
        } else if (piece.label == "相" || piece.label == "象" || piece.label == "仕" || piece.label == "士") {
            value = 2.0;
        }

        if (piece.isRed) {
            redScore += value;
        } else {
            blackScore += value;
        }
    }

    const double total = redScore + blackScore;
    if (total == 0) {
        return 0.5;
    }
    return redScore / total;
}
