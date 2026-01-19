#ifndef PAWN_H
#define PAWN_H

#include "../include/Piece.h"

class Pawn_Piece : public Piece
{
public:
    // Hàm khởi tạo của quân Tốt
    Pawn_Piece(Color c, int x, int y) : Piece(Name::Pawn, c, x, y) {}
    ~Pawn_Piece();
};

#endif