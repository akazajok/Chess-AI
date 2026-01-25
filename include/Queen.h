#ifndef QUEEN_H
#define QUEEN_H

#include "../include/Piece.h"
#include "../include/Board.h"
#include <cstdlib> // hàm abs()

class Queen : public Piece
{
public:
    Queen(Color color, int row, int col) : Piece(Name::Queen, color, row, col) {};
    bool Is_Valid_Move(int destRow, int destCol, const Board &grid) override;
};

#endif