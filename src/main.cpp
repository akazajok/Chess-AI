#include <iostream>
#include <string>
#include "../include/Board.h"

using namespace std;

int main()
{
    // 1. Tạo đối tượng bàn cờ
    Board chessGame;

    // 2. Tạo một chuỗi string đại diện cho bàn cờ (8x8 = 64 ký tự)
    // Chữ hoa: Trắng, Chữ thường: Đen, Dấu chấm: Ô trống
    // Đây là cách cực kỳ nhanh để test các trường hợp đặc biệt
    string testLayout = "8/8/8/8/8/k7/P7/8 w - - 0 1";
    // 3. Nạp thế cờ vào bàn cờ
    chessGame.Set_Up_Board(testLayout);
    // 4. Hiển thị kết quả để kiểm tra
    chessGame.Display();
    chessGame.Execute_Move(6, 0, 4, 0);
    chessGame.Display();
    cout << "\nTest thanh cong!" << endl;

    return 0;
}