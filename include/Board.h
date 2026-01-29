#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <functional>
#include <array>
#include <map>
#include <string>
#include <memory>
#include <sstream>
#include <cstdlib> // hàm abs()
#include <utility> // pair

#include "../include/Piece.h"

#include "../include/Pawn.h"
#include "../include/Rook.h"
#include "../include/Bishop.h"
#include "../include/Knight.h"
#include "../include/Queen.h"
#include "../include/King.h"

using BoardGrid = std::array<std::array<std::unique_ptr<Piece>, 8>, 8>;
class Board
{
private:
    BoardGrid grid;
    char sideToMove;             // Ai là người đi tiếp theo?
    std::string castlingRights;  // Quyền nhập thành ( vua, xe chưa di chuyển )
    std::string enPassantTarget; // Bắt tốt qua đường
    int halfmoveClock;           // Luật 50 nước
    int fullmoveNumber;          // Số lượt đi của ván đấu

public:
    int rowKingBlack, colKingBlack; // vị trí quân vua phe đen
    int rowKingWhite, colKingWhite; // vị trí quân vua phe trắng

    // hàm khởi tạo bàn cờ theo yêu cầu ( nhập dữ liệu chữ )
    void Set_Up_Board(std::string &FEN);
    void Display(); // Hàm kiểm tra
    // Hàm kiểm tra ngoại lệ && đúng luật
    bool Can_Move(int startRow, int startCol, int destRow, int destCol);
    // Hàm di chuyển quân cờ
    void Update_Position(int startRow, int startCol, int destRow, int destCol);
    // Thực thi di chuyển quân cờ
    void Execute_Move(int startRow, int startCol, int destRow, int destCol);
    // Lấy quân cờ chặn đường
    Piece *Get_Piece_On_Path(int startRow, int startCol, int destRow, int destCol);
    // Hàm kiểm tra vua có bị chiếu tướng không ?
    bool Is_King_In_Check(int rowKing, int colKing, Color color);
    // Hàm lấy dữ liệu quân cờ
    Piece *Get_Piece_At(int row, int col) const
    {
        if (row < 0 || row > 7 || col < 0 || col > 7)
            return nullptr;
        return grid[row][col].get();
    }
};

#endif
