#include "../include/Pawn.h"

bool Pawn::Is_Valid_Move(int destRow, int destCol, const Board &grid)
{
    // quân đen đi xuống, quân trắng đi lên
    int step = (color == Color::Black) ? 1 : -1;

    // kiểm tra quân đã di chuyển hay chưa
    bool isAtStartingRow = (color == Color::Black && row == 1) || (color == Color::White && row == 6);

    // đi thẳng 2 ô ( nước đầu )
    if (isAtStartingRow && col == destCol && destRow == row + 2 * step)
        return grid.Get_Piece_At(row + step, destCol) == nullptr &&
               grid.Get_Piece_At(destRow, destCol) == nullptr;

    // đi 1 ô
    else if (col == destCol && destRow == row + step)
        return grid.Get_Piece_At(destRow, destCol) == nullptr;

    // ăn chéo
    else if (abs(destCol - col) == 1 && destRow == row + step)
        return grid.Get_Piece_At(destRow, destCol) != nullptr;

    return false;
}