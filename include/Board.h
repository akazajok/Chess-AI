#ifndef BOARD_H
#define BOARD_H

#include <string>
#include "../include/Piece.h"

class Board
{
private:
    Piece *grid[8][8];

public:
    Board();
    ~Board();

    void Set_Up_Board(std::string board_test); // Hàm khởi tạo bàn cờ theo yêu cầu
    void Display();                       // Hàm kiểm tra
};

#endif
