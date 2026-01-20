#ifndef PIECE_H
#define PIECE_H

#include <cstdint>
#include <utility>

enum class Color : std::uint8_t
{
    White,
    Black,
    None
};
enum class Name : std::uint8_t
{
    Pawn,
    Rook,
    Bishop,
    Knight,
    Queen,
    King
};

class Piece
{
protected:
    int row, col; // tọa độ trên bàn cờ ( 0 -> 7 )
    Color color;
    Name name;

public:
    Piece(Name, Color, int row, int col);
    virtual ~Piece(); // xóa bộ nhớ

    Name Get_Name();                    // Hàm xác định quân gì
    Color Get_Color();                  // Hàm lấy màu quân cờ
    std::pair<int, int> Get_Position(); // Hàm lấy tọa độ quân cờ
    void Set_Position(int row, int col);
    // Hàm thuần ảo: Ép các quân cờ con (Xe, Tượng...) phải tự định nghĩa cách đi riêng
    // virtual bool IsValidMove(int destX, int destY, Piece *board[8][8]) = 0;
};

#endif
