#ifndef BISHOP_H
#define BISHOP_H

#include "../include/Piece.h"

class Bishop : public Piece
{
public:
    Bishop(Color color, int row, int col) : Piece(Name::Bishop, color, row, col) {};
};

#endif