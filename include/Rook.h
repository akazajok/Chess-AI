#ifndef ROOK_H
#define ROOK_H

#include "../include/Piece.h"
#include "../include/Board.h"

class Board;
class Rook : public Piece
{
public:
    Rook(Color color, int row, int col) : Piece(Name::Rook, color, row, col) {};
    bool Is_Valid_Move(int destRow, int destCol, const Board &grid) override;
};

#endif