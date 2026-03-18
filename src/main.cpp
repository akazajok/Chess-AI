#include "../include/gamePlay.h"

int main()
{
    // Chuỗi FEN khởi tạo bàn cờ mặc định
    std::string defaultFEN = "2k5/8/6p1/6q1/7b/8/3Br2K/5q2 w - - 3 18";

    gameManager manager;
    manager.Init_Game(defaultFEN, GameMode::PvP, 20);

    std::string moveStr;
    while (std::cin >> moveStr)
    {
        if (moveStr == "reset")
            manager.Init_Game(defaultFEN, GameMode::PvP, 20);

        std::string result = manager.Process_Web_Move(moveStr);

        // In chuỗi fen mới nhất ra màn hình
        std::cout << result << std::endl;
    }
    return 0;
}