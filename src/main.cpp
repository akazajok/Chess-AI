#include "../include/gamePlay.h"

int main()
{
    // Chuỗi FEN khởi tạo bàn cờ mặc định
    std::string defaultFEN = "7k/8/8/8/1P6/8/2P5/1K6 w - - 0 1";

    gameManager manager;
    manager.Init_Game(defaultFEN); // Nạp dữ liệu vào bàn cờ bên trong manager
    manager.Game_Turn();           // Bắt đầu vòng lặp game

    return 0;
}