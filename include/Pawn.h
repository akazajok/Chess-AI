#ifndef PAWN_H
#define PAWN_H

#include "../include/Piece.h"

class Pawn : public Piece
{
public:
    // Hàm khởi tạo của quân Tốt
    Pawn(Color color, int row, int col) : Piece(Name::Pawn, color, row, col) {}
    //~Pawn_Piece();
};

#endif