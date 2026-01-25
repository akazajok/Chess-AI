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

bool Piece::Is_Path_Clear(int destRow, int destCol, const Board &grid)
{
    // Xác định hướng di chuyển của xe hay tịnh
    int stepRow = (row == destRow) ? 0 : (destRow > row) ? 1
                                                         : -1;
    int stepCol = (col == destCol) ? 0 : (destCol > col) ? 1
                                                         : -1;
    int currR = row + stepRow, currC = col + stepCol;

    // Chạy vòng lặp kiểm tra các ô ở giữa (không kiểm tra ô đích)
    while (currR != destRow || currC != destCol)
    {
        if (grid.Get_Piece_At(currR, currC) != nullptr)
            return false; // Thấy vật cản

        currR += stepRow;
        currC += stepCol;
    }
    return true;
}