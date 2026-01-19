#include "../include/Piece.h"

Piece::Piece(Name name, Color color, int x, int y) : name(name), color(color), x(x), y(y) {}

Piece::~Piece() {}

Name Piece::Get_Name()
{
    return name;
}

Color Piece::Get_Color()
{
    return color;
}

int Piece::Get_Piece_Coordinate()
{
    return x, y;
}