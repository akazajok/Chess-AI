#include "../include/Rook.h"

bool Rook::Is_Valid_Move(int destRow, int destCol, const Board &grid)
{
    if (row == destRow || col == destCol)
    {
        return Is_Path_Clear(destRow, destCol, grid);
    }
    return false;
}