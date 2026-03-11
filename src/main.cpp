#include "../include/gamePlay.h"

int main()
{
    // Chuỗi FEN khởi tạo bàn cờ mặc định
    std::string defaultFEN = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1";

    gameManager manager;

    std::cout << "=== CHAO MUNG DEN VOI CHESS AI ===\n";
    std::cout << "1. Choi voi ban be (PvP)\n";
    std::cout << "2. Choi voi AI (PvE)\n";
    std::cout << "Chon che do (1/2): ";

    int choice;
    std::cin >> choice;

    GameMode mode = (choice == 2) ? GameMode::PvE : GameMode::PvP;
    int aiLevel = 20; // Mặc định là Đại kiện tướng
    if (mode == GameMode::PvE)
    {
        std::cout << "\nChon do kho AI:\n";
        std::cout << "1. De\n2. Trung binh\n3. Kho\n4. Dai kien tuong\n";
        std::cout << "Lua chon cua ban: ";
        int levelChoice;
        std::cin >> levelChoice;
        switch (levelChoice)
        {
        case 1:
            aiLevel = 1;
            break;
        case 2:
            aiLevel = 6;
            break;
        case 3:
            aiLevel = 14;
            break;
        case 4:
            aiLevel = 20;
            break;
        default:
            aiLevel = 6;
            break; // Xử lý nếu nhập sai
        }
    }

    std::string compe;
    std::cout << "Ban co muon choi ban co cua rieng minh ?\n";
    std::cout << "yes/no\n";
    std::cin >> compe;
    if (to_lower(compe) == "yes")
    {
        std::cout << "Nhap URL hinh anh\n";
        std::cin >> defaultFEN;
    }
    // Sau đó bạn cần sửa hàm Init_Game để truyền thêm biến aiLevel này vào
    manager.Init_Game(defaultFEN, mode, aiLevel);
    manager.Game_Turn();

    return 0;
}