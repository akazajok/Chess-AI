#ifndef ROOK_H
#define ROOK_H

#include "../include/Piece.h"

class Rook : public Piece
{
public:
    Rook(Color color, int row, int col) : Piece(Name::Rook, color, row, col) {};
    bool Is_Valid_Move(int destX, int destY, const Board &grid) override;
};

#endif