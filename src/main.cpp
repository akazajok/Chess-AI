#include <iostream>
#include <string>
#include "../include/Board.h"

using namespace std;

int main()
{
    Board chessGame;

    string testLayout = "8/8/8/8/8/8/3N4/8 w - - 0 1";

    chessGame.Set_Up_Board(testLayout);

    chessGame.Display();
    chessGame.Execute_Move(6, 3, 4, 4);
    chessGame.Display();
    cout << "\nTest thanh cong!" << endl;

    return 0;
}