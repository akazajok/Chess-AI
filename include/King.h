#ifndef KING_H
#define KING_H

#include "../include/Piece.h"
#include "../include/Board.h"
#include <cstdlib> 
class King : public Piece
{
public:
    King(Color color, int row, int col) : Piece(Name::King, color, row, col) {};
    bool Is_Valid_Move(int destX, int destY, const Board &grid) override;
};

#endif