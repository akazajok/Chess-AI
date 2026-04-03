#include "../include/gamePlay.h"

int main()
{
    // Chuỗi FEN khởi tạo bàn cờ mặc định
    std::string defaultFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

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