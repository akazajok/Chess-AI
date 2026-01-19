#include "../include/Board.h"
#include "../include/Pawn.h"
#include <iostream>

Board::Board()
{
    for (int i = 0; i <= 7; i++)
    {
        for (int j = 0; j <= 7; j++)
        {
            grid[i][j] = nullptr;
        }
    }
}

void Board::Set_Up_Board(std::string board_test)
{
    int j = 0;
    for (int i = 0; i < board_test.size(); i++)
    {
        switch (board_test[i])
        {
        case 'p':
            grid[j][i] = new Pawn_Piece(Black, j, i);
            break;
        default:
            break;
        }
    }
}

// 4. Hàm hiển thị để kiểm tra
void Board::Display()
{
    std::cout << "  0 1 2 3 4 5 6 7" << '\n';
    for (int i = 0; i < 8; i++)
    {
        std::cout << i << " ";
        for (int j = 0; j < 8; j++)
        {
            if (grid[i][j] == nullptr)
            {
                std::cout << "--";
            }
            else
            {
                // Ở đây bạn có thể dùng GetName() để quyết định in chữ gì
                if (grid[i][j]->Get_Name() == Pawn)
                    std::cout << (grid[i][j]->Get_Color() == White ? "P " : "p ");
                else if (grid[i][j]->Get_Name() == Rook)
                    std::cout << (grid[i][j]->Get_Color() == White ? "R " : "r ");
                // ... tương tự cho các quân khác
            }
        }
        std::cout << '\n';
    }
}

// rnbqkbnr / pppppppp / 8 / 8 / 8 / 8 / PPPPPPPP / RNBQKBNR w KQkq - 0 1