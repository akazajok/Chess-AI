#include "../include/Utils.h"

std::string convert_from_XY(const int &hang, const int &cot)
{
    char kitu_cot = cot + 97;
    char kitu_hang = '8' - hang;
    std::string ketqua = std::string(1, kitu_cot) + std::string(1, kitu_hang);
    return ketqua;
}

std::pair<int, int> convert_to_XY(std::string coord)
{
    char cot_chu = coord[0];
    char hang_chu = coord[1];
    int cot = cot_chu - 'a';
    int hang = '8' - hang_chu;
    return std::make_pair(hang, cot);
}

void updatePosition(std::string board[8][8], std::string move)
{
    std::string first_move = move.substr(0, 2);
    std::string last_move = move.substr(2, 2);
    std::pair<int, int> start = convert_to_XY(first_move);
    std::pair<int, int> end = convert_to_XY(last_move);
    std::string current = board[start.first][start.second];
    board[end.first][end.second] = current;
    board[start.first][start.second] = " .";
}
bool isValidMoveFormat(std::string move)
{
    if (move.length() != 4 && move.length() != 5)
        return false;

    if (move[0] < 'a' || move[0] > 'h' ||
        move[1] < '1' || move[1] > '8' ||
        move[2] < 'a' || move[2] > 'h' ||
        move[3] < '1' || move[3] > '8')
        return false;

    if (move.length() == 5)
    {
        char p = tolower(move[4]);
        if (p != 'q' && p != 'r' && p != 'b' && p != 'n')
        {
            return false;
        }
    }

    return true;
}
// Trong utils.cpp
std::string to_lower(std::string s)
{
    for (char &c : s)
        c = std::tolower((unsigned char)c);
    return s;
}