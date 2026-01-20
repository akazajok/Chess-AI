#ifndef KING_H
#define KING_H

#include "../include/Piece.h"

class King : public Piece
{
public:
    King(Color color, int row, int col) : Piece(Name::King, color, row, col) {};
};

#endif