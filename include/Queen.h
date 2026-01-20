#ifndef QUEEN_H
#define QUEEN_H

#include "../include/Piece.h"

class Queen : public Piece
{
public:
    Queen(Color color, int row, int col) : Piece(Name::Queen, color, row, col) {};
};

#endif