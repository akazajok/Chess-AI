#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "Board.h" // Bao gồm Board.h để gameManager sử dụng class Board
#include "utils.h" // Sử dụng các hàm chuyển đổi tọa độ
#include <string>

class gameManager
{
private:
    Board chessGame; // Đối tượng bàn cờ duy nhất của trò chơi
    int rowKing, colKing;
    Color colorKing;

public:
    gameManager(); // Khởi tạo các giá trị mặc định

    // Nạp dữ liệu bàn cờ từ chuỗi FEN
    void Init_Game(std::string FEN);

    // Kiểm tra tính hợp lệ của nước đi nhập từ bàn phím
    bool Is_Valid_Input(const std::string &moveStr);

    // Vòng lặp điều khiển lượt chơi và hiển thị
    void Game_Turn();
};

#endif