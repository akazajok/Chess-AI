#include <iostream>
#include <string>
#include "../include/Board.h"

using namespace std;

int main()
{
    Board chessGame;

    string testLayout = "8/8/8/8/8/3Q4/8/8 w - - 0 1";

    chessGame.Set_Up_Board(testLayout);

    chessGame.Display();
    chessGame.Execute_Move(5, 3, 0, 3);
    chessGame.Display();
    cout << "\nTest thanh cong!" << endl;

    return 0;
}