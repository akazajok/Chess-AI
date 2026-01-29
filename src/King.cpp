#include "../include/King.h"

bool King::Is_Valid_Move(int destRow, int destCol, const Board &grid)
{
    if (abs(row - destRow) <= 1 && abs(col - destCol) <= 1 &&
        (abs(row - destRow) != 0 || abs(col - destCol) != 0))
        return true;
    return false;
}