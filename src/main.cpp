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

        Piece *pieceCheckWhite = chessGame.Get_Checking_Piece(chessGame.rowKingWhite, chessGame.colKingWhite, Color::White);
        Piece *pieceCheckBlack = chessGame.Get_Checking_Piece(chessGame.rowKingBlack, chessGame.colKingBlack, Color::Black);

        bool isCheckMate = chessGame.Can_Escape_Check(chessGame.rowKingBlack, chessGame.colKingBlack, Color::Black);

        cout << "Test case :" << " " << cnt << '\n';
        cout << testLayout << '\n';
        cout << "White King in check: " << pieceCheckWhite << '\n';
        cout << "Black King in check: " << pieceCheckBlack << '\n';

        cout << "Có thể thoát chiếu tướng không: " << " " << isCheckMate << '\n';

        cout << '\n';

        cnt++;
    }
    return 0;
}