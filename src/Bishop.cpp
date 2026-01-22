#include "../include/Bishop.h"

bool Bishop::Is_Valid_Move(int destRow, int destCol, const Board &grid)
{
    // hàm kiểm tra có vật cản hay không ???
    auto Is_Path_Blocked = [&]() -> bool
    {
        int stepRow = (destRow < row) ? -1 : 1; // di chuyển lên trên hay xuống dưới
        int stepCOl = (destCol > col) ? 1 : -1; // di chuyển sang trái hay phải
        int currR = row + stepRow, currC = col + stepCOl;
        while (currR != destRow && currC != destCol)
        {
            if (grid.Get_Piece_At(currR, currC) != nullptr)
                return true;
            currR += stepRow;
            currC += stepCOl;
        }
        return false;
    };
    if (abs(row - destRow) == abs(col - destCol)) // điểm đến có đúng luật không ???
    {
        if (Is_Path_Blocked()) // có vật cản
            return false;
        return true;
    }
    return false;
}