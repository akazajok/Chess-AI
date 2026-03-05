#ifndef UTILS_H
#define UTILS_H

#include "../include/Piece.h"
#include <string>
#include <utility> // Thư viện chứa pair
#include <cctype>  // Thư viện chứa hàm tolower

std::string convert_from_XY(const int &hang, const int &cot);
std::pair<int, int> convert_to_XY(std::string coord);
void updatePosition(std::string board[8][8], std::string move);
bool isValidMoveFormat(std::string move);

std::string to_lower(std::string s);

#endif
