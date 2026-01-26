#include "../include/Knight.h"

bool Knight::Is_Valid_Move(int destRow, int destCol, const Board &grid)
{
    if ((abs(col - destCol) == 1 && abs(row - destRow) == 2) ||
        (abs(col - destCol) == 2 && abs(row - destRow) == 1))
        return true;

    return false;
}