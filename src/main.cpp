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

        cout << "Test case :" << " " << cnt << " ";
        cout << testLayout << '\n';

        if (chessGame.rowKingWhite >= 0 && chessGame.rowKingWhite <= 7)
        {
            Piece *pieceCheckWhite = chessGame.Get_Checking_Piece(chessGame.rowKingWhite, chessGame.colKingWhite, Color::White);

            cout << "White King in check: " << pieceCheckWhite << '\n';
            cout << "Số quân đang chiếu vua trắng : " << chessGame.cntCheck << '\n';

            bool isCheckMateWhite = pieceCheckWhite != nullptr ? chessGame.Can_Escape_Check(chessGame.rowKingWhite, chessGame.colKingWhite, Color::White) : 1;

            cout << "Có thể thoát chiếu tướng không: " << " " << isCheckMateWhite << '\n';
        }

        if (chessGame.rowKingBlack >= 0 && chessGame.rowKingBlack <= 7)
        {

            Piece *pieceCheckBlack = chessGame.Get_Checking_Piece(chessGame.rowKingBlack, chessGame.colKingBlack, Color::Black);

            cout << "Black King in check: " << pieceCheckBlack << '\n';
            cout << "Số quân đang chiếu vua đen : " << chessGame.cntCheck << '\n';

            bool isCheckMateBlack = pieceCheckBlack != nullptr ? chessGame.Can_Escape_Check(chessGame.rowKingBlack, chessGame.colKingBlack, Color::Black) : 1;

            cout << "Có thể thoát chiếu tướng không: " << " " << isCheckMateBlack << '\n';
        }

        cout << '\n';

        cnt++;
    }

    return 0;
}