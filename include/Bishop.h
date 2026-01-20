#ifndef BISHOP_H
#define BISHOP_H

#include "../include/Piece.h"

class Bishop : public Piece
{
public:
    Bishop(Color color, int row, int col) : Piece(Name::Bishop, color, row, col) {};
    bool Is_Valid_Move(int destX, int destY, Piece *grid[8][8]) override;
};

#endif