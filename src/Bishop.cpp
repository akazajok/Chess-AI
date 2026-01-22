#include "../include/Bishop.h"

bool Bishop::Is_Valid_Move(int destRow, int destCol, const Board &grid)
{
    if (abs(row - destRow) == abs(col - destCol)) // điểm đến có đúng luật không ???
    {
        // Kiểm tra xem có vật cản đường không ???
        int stepRow = (destRow < row) ? -1 : 1; // di chuyển lên trên hay xuống dưới
        int stepCOl = (destCol > col) ? 1 : -1; // di chuyển sang trái hay phải
        int currR = row + stepRow, currC = col + stepCOl;
        while (currR != destRow && currC != destCol)
        {
            if (grid.Get_Piece_At(currR, currC) != nullptr)
                return false;
            currR += stepRow;
            currC += stepCOl;
        }
        return true;
    }
    return false;
}