#ifndef UTILS_H
#define UTILS_H

#include "../include/Piece.h"
#include <string>
#include <utility> // Thư viện chứa pair

std::string convert_from_XY(const int &hang, const int &cot);
std::pair<int, int> convert_to_XY(std::string coord);
void updatePosition(std::string board[8][8], std::string move);
bool isValidMoveFormat(std::string move);

#endif
