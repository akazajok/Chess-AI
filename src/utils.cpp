#include <bits/stdc++.h>
using namespace std;

string convert_from_XY(int hang, int cot)
{
    char kitu_cot = cot + 97;
    char kitu_hang = '8' - hang;
    string ketqua = string(1, kitu_cot) + string(1, kitu_hang);
    return ketqua;
}

pair<int, int> convert_to_XY(string coord)
{
    char cot_chu = coord[0];
    char hang_chu = coord[1];
    int cot = cot_chu - 'a';
    int hang = '8' - hang_chu;
    return make_pair(hang, cot);
}

void updatePosition(string board[8][8], string move)
{
    string first_move = move.substr(0, 2);
    string last_move = move.substr(2, 2);
    pair<int, int> start = convert_to_XY(first_move);
    pair<int, int> end = convert_to_XY(last_move);
    string current = board[start.first][start.second];
    board[end.first][end.second] = current;
    board[start.first][start.second] = " .";
}
bool isValidMoveFormat(string move)
{
    if (move.length() == 4 &&
        move[0] >= 'a' && move[0] <= 'h' &&
        move[1] >= '1' && move[1] <= '8' &&
        move[2] >= 'a' && move[2] <= 'h' &&
        move[3] >= '1' && move[3] <= '8')
        return true;
    else
        return false;
}
void displayBoard(string board[8][8])
{
    cout << "   a  b  c  d  e  f  g  h\n";
    for (int i = 0; i < 8; i++)
    {
        cout << (8 - i) << " ";
        for (int j = 0; j < 8; j++)
        {
            cout << board[i][j] << " ";
        }
        cout << (8 - i) << " " << endl;
    }
    cout << "   a  b  c  d  e  f  g  h\n";
}
