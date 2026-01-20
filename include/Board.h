#ifndef BOARD_H
#define BOARD_H

#include <bits/stdc++.h>
#include <functional>

#include "../include/Piece.h"

#include "../include/Pawn.h"
#include "../include/Rook.h"
#include "../include/Bishop.h"
#include "../include/Knight.h"
#include "../include/Queen.h"
#include "../include/King.h"

class Board
{
private:
    Piece *grid[8][8];

public:
    Board();  // hàm khởi tạo bàn cờ
    ~Board(); // hàm xóa bộ nhớ

    // hàm khởi tạo bàn cờ theo yêu cầu ( nhập dữ liệu chữ )
    void Set_Up_Board(std::string board_test);
    void Display(); // Hàm kiểm tra

    // Hàm di chuyển quân cờ
    void Update_Position(int startRow, int startCol, int endRow, int endCol);
};

#endif
