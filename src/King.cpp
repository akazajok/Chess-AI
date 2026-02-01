#include "../include/King.h"

bool King::Is_Valid_Move(int destRow, int destCol, const Board &grid)
{
    // Normal king move
    if (abs(row - destRow) <= 1 && abs(col - destCol) <= 1 &&
        (abs(row - destRow) != 0 || abs(col - destCol) != 0))
        return true;
    // Castling kings move
    if (destRow == row && abs(destCol - col) == 2)
        return true;

    return false;
}