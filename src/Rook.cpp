#include "../include/Rook.h"

bool Rook::Is_Valid_Move(int destRow, int destCol, const Board &grid)
{
    if (row == destRow || col == destCol)
    {
        int stepRow = 0, stepCol = 0;

        // di chuyển ngang hay đi thẳng
        if (row != destRow)
            stepRow = (destRow > row) ? 1 : -1;
        else
            stepCol = (destCol > col) ? 1 : -1;

        int currR = row + stepRow, currC = col + stepCol;
        // kiểm tra quân cờ có bị chặn không
        while (currR != destRow || currC != destCol)
        {
            if (grid.Get_Piece_At(currR, currC) != nullptr)
                return false;

            currC += stepCol;
            currR += stepRow;
        }
        return true;
    }
    return false;
}