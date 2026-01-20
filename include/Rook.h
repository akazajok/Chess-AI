#ifndef ROOK_H
#define ROOK_H

#include "../include/Piece.h"

class Rook : public Piece
{
public:
    Rook(Color color, int row, int col) : Piece(Name::Rook, color, row, col) {};
};

#endif