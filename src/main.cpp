#include "../include/gamePlay.h"

int main()
{
    // Chuỗi FEN khởi tạo bàn cờ mặc định
<<<<<<< HEAD
    std::string defaultFEN = "7k/8/8/8/1P6/8/2P5/1K6 w - - 0 1";

    gameManager manager;
    manager.Init_Game(defaultFEN); // Nạp dữ liệu vào bàn cờ bên trong manager
    manager.Game_Turn();           // Bắt đầu vòng lặp game
=======
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
>>>>>>> dd9fad3b236177065cd05907461c1013155ccd7f

    return 0;
}