#include <iostream>
#include <string>
#include "../include/Board.h"

using namespace std;

int main()
{
    Board chessGame;
    
    // Test đơn giản: Knight ở góc bàn cờ
    cout << "=== TEST KNIGHT SIMPLE ===" << endl;
    
    // Đặt Knight ở a1 (góc dưới trái)
    string simpleLayout = "8/8/8/8/8/8/8/N7 w - - 0 1";
    chessGame.Set_Up_Board(simpleLayout);
    
    cout << "Knight o a1 (row=7, col=0):" << endl;
    chessGame.Display();
    
    // Test 1: Di chuyển từ a1 đến b3 (1 phải, 2 lên) - HỢP LỆ
    cout << "\n--- Test: a1 -> b3 (should work) ---" << endl;
    cout << "Tu (7,0) den (5,1):" << endl;
    chessGame.Check_Move(7, 0, 5, 1);
    chessGame.Display();
    
    // Reset
    chessGame.Set_Up_Board(simpleLayout);
    
    // Test 2: Di chuyển từ a1 đến c2 (2 phải, 1 lên) - HỢP LỆ
    cout << "\n--- Test: a1 -> c2 (should work) ---" << endl;  
    cout << "Tu (7,0) den (6,2):" << endl;
    chessGame.Check_Move(7, 0, 6, 2);
    chessGame.Display();
    
    // Reset
    chessGame.Set_Up_Board(simpleLayout);
    
    // Test 3: Di chuyển từ a1 đến a3 (thẳng hàng) - KHÔNG HỢP LỆ
    cout << "\n--- Test: a1 -> a3 (should fail) ---" << endl;
    cout << "Tu (7,0) den (5,0):" << endl;
    chessGame.Check_Move(7, 0, 5, 0);
    chessGame.Display();
    cout << "Knight should stay at a1" << endl;

    return 0;
}