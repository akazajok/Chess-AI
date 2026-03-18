#include "../include/Bishop.h"

bool Bishop::Is_Valid_Move(int destRow, int destCol, const Board &grid)
{
    if (abs(row - destRow) == abs(col - destCol)) // điểm đến có đúng luật không ???
        return true;
    return false;
}

std::vector<MoveInfor> Bishop::getValidMoves(int row, int col, const Board &grid)
{
    std::vector<MoveInfor> validMoves;
    Piece *p = grid.Get_Piece_At(row, col);
    int stepRow[] = {1, 1, -1, -1};
    int stepCol[] = {1, -1, 1, -1};

    for (int i = 0; i < 4; ++i)
    {
        int r = row, c = col;
        while (r >= 0 && c >= 0 && r < 8 && c < 8)
        {
            r += stepRow[i];
            c += stepCol[i];

            Piece *targetPiece = grid.Get_Piece_At(r, c);
            std::string id = convert_from_XY(r, c);

            if (!targetPiece)
                validMoves.push_back({id, false});
            else
            {
                if (targetPiece->Get_Color() != p->Get_Color())
                    validMoves.push_back({id, true});

                break;
            }
        }
    }
    return validMoves;
}