#ifndef KNIGHT_H
#define KNIGHT_H

#include "../include/Piece.h"
#include "../include/Board.h"
#include <cstdlib>

class Knight : public Piece
{
public:
    Knight(Color color, int row, int col) : Piece(Name::Knight, color, row, col) {};
    bool Is_Valid_Move(int destRow, int destCol, const Board &grid) override;
};

#endif