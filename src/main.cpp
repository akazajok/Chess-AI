#include <iostream>
#include <string>
#include "../include/Board.h"

using namespace std;

int main()
{
    Board chessGame;
    
    cout << "Simple castling test:\n";
    string test = "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1";
    chessGame.Set_Up_Board(test);
    
    cout << "Before castling:\n";
    chessGame.Display();
    
    cout << "Attempting White Kingside Castling...\n";
    chessGame.Execute_Move(7, 4, 7, 6);
    
    cout << "After castling:\n";
    chessGame.Display();
    
    return 0;
}