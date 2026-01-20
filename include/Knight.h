#ifndef KNIGHT_H
#define KNIGHT_H

#include "../include/Piece.h"

class Knight : public Piece
{
public:
    Knight(Color color, int row, int col) : Piece(Name::Knight, color, row, col) {};
};

#endif