#include "../include/Piece.h"
#include "../include/Board.h"

Piece::Piece(Name name, Color color, int row, int col) : name(name), color(color), row(row), col(col) {}

Piece::~Piece() {}

Name Piece::Get_Name() const
{
    return name;
}

Color Piece::Get_Color() const
{
    return color;
}

std::pair<int, int> Piece::Get_Position() const
{
    return {row, col};
}
void Piece::Set_Position(int row, int col)
{
    this->row = row;
    this->col = col;
}

std::vector<MoveInfor> Piece::getValidMoves(int row, int col, const Board &grid)
{
    std::vector<MoveInfor> validMoves;
    Piece *piece = grid.Get_Piece_At(row, col);

    for (int r = 0; r < 8; ++r)
    {
        for (int c = 0; c < 8; ++c)
        {
            if (piece->Is_Valid_Move(r, c, grid))
            {
                Piece *target = grid.Get_Piece_At(r, c);
                std::string id = convert_from_XY(r, c);

                if (!target)
                {
                    validMoves.push_back({id, false});
                }
                else
                {
                    if (target->Get_Color() != piece->Get_Color())
                        validMoves.push_back({id, true});
                }
            }
        }
    }

    return validMoves;
}