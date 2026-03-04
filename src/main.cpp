#include "../include/gamePlay.h"

int main()
{
    // Chuỗi FEN khởi tạo bàn cờ mặc định
    std::string defaultFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    gameManager manager;

    std::cout << "=== CHAO MUNG DEN VOI CHESS AI ===\n";
    std::cout << "1. Choi voi ban be (PvP)\n";
    std::cout << "2. Choi với AI (PvE)\n";
    std::cout << "Chon che do (1/2): ";

    int choice;
    std::cin >> choice;

    GameMode mode = (choice == 2) ? GameMode::PvE : GameMode::PvP;

    manager.Init_Game(defaultFEN, mode);
    manager.Game_Turn();

    return 0;
}