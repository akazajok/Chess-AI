#include "../include/Queen.h"

bool Queen::Is_Valid_Move(int destRow, int destCol, const Board &grid)
{
    bool isStraight = (row == destRow || col == destCol);
    bool isDiagonal = (abs(row - destRow) == abs(col - destCol));

    // Nếu không đi đúng hướng của Xe hoặc Tượng thì sai
    if (!isStraight && !isDiagonal)
        return false;

    // Nếu đúng hướng, gọi hàm check vật cản
    return Is_Path_Clear(destRow, destCol, grid);
}