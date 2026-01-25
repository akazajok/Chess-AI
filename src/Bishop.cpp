#include "../include/Bishop.h"

bool Bishop::Is_Valid_Move(int destRow, int destCol, const Board &grid)
{
    if (abs(row - destRow) == abs(col - destCol)) // điểm đến có đúng luật không ???
    {
        return Is_Path_Clear(destRow, destCol, grid);
    }
    return false;
}