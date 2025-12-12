#pragma once

#include <QPoint>
#include <QString>
#include <optional>
#include <vector>

struct Move {
    QPoint from;
    QPoint to;
    QString capturedLabel;
    bool capturedRed{false};
};

struct PieceSnapshot {
    QPoint position;
    QString label;
    bool isRed{true};
};

class BoardState {
public:
    BoardState();

    std::optional<PieceSnapshot> pieceAt(const QPoint &coord) const;
    std::optional<Move> createMove(const QPoint &from, const QPoint &to) const;
    void applyMove(const Move &move);
    bool undo();

    bool redToMove() const { return m_redToMove; }
    bool isCheckmate() const;

    const std::vector<PieceSnapshot> &pieces() const { return m_pieces; }

private:
    bool isInsidePalace(const QPoint &pos, bool red) const;
    bool validateMove(const PieceSnapshot &piece, const QPoint &to) const;
    bool pathClear(const QPoint &from, const QPoint &to) const;
    int countPiecesBetween(const QPoint &from, const QPoint &to) const;

    std::vector<PieceSnapshot> m_pieces;
    std::vector<Move> m_history;
    bool m_redToMove{true};
};
