#include "../include/King.h"

bool King::Is_Valid_Move(int destX, int destY, const Board &grid) {
    if (abs(row - destX) <= 1 && abs(col-destY) <= 1 && 
    (abs(row - destX) != 0 || abs(col-destY) != 0))
        return true;
    return false;   
}