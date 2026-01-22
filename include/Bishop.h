#ifndef BISHOP_H
#define BISHOP_H

#include "../include/Piece.h"
#include "../include/Board.h"

#include <cstdlib> // hàm abs()
class Bishop : public Piece
{
public:
    Bishop(Color color, int row, int col) : Piece(Name::Bishop, color, row, col) {};
    bool Is_Valid_Move(int destX, int destY, const Board &grid) override;
};

#endif