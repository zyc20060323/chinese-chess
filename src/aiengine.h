#pragma once

#include "boardstate.h"

#include <optional>

class AIEngine {
public:
    AIEngine() = default;

    std::optional<Move> chooseMove(const BoardState &state) const;
    double evaluate(const BoardState &state) const;
};
