#include <iostream>
#include <string>
#include "../include/Board.h"

using namespace std;

int main()
{
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    freopen("src/main.inp", "r", stdin);
    freopen("src/main.out", "w", stdout);

    int cnt = 1;
    string testLayout;
    while (getline(cin, testLayout))
    {
        if (testLayout.empty())
            continue;

        Board chessGame;

        chessGame.Set_Up_Board(testLayout);
        // chessGame.Display();

        bool isWhiteInCheck = chessGame.Is_King_In_Check(chessGame.rowKingWhite, chessGame.colKingWhite, Color::White);
        bool isBlackInCheck = chessGame.Is_King_In_Check(chessGame.rowKingBlack, chessGame.colKingBlack, Color::Black);

        cout << "Test case :" << " " << cnt << '\n';
        cout << testLayout << '\n';
        cout << "White King in check: " << isWhiteInCheck << '\n';
        cout << "Black King in check: " << isBlackInCheck << '\n'
             << '\n';
        cnt++;
    }
    return 0;
}