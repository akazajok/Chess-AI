#include "../include/Piece.h"
#include "../include/Board.h"

Piece::Piece(Name name, Color color, int row, int col) : name(name), color(color), row(row), col(col) {}

Piece::~Piece() {}

Name Piece::Get_Name() const
{
    return name;
}

Color Piece::Get_Color() const
{
    return color;
}

std::pair<int, int> Piece::Get_Position() const
{
    return {row, col};
}
void Piece::Set_Position(int row, int col)
{
    this->row = row;
    this->col = col;
}