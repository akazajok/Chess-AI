#include "../include/Piece.h"

Piece::Piece(Name name, Color color, int row, int col) : name(name), color(color), row(row), col(col) {}

Piece::~Piece() {}

Name Piece::Get_Name()
{
    return name;
}

Color Piece::Get_Color()
{
    return color;
}

std::pair<int, int> Piece::Get_Position()
{
    return {row, col};
}
void Piece::Set_Position(int row, int col)
{
    this->row = row;
    this->col = col;
}