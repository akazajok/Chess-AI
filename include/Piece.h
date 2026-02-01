#ifndef PIECE_H
#define PIECE_H

#include <cstdint>
#include <utility>

class Board;

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
    King,
    None
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

    Name Get_Name() const;                    // Hàm xác định quân gì
    Color Get_Color() const;                  // Hàm lấy màu quân cờ
    std::pair<int, int> Get_Position() const; // Hàm lấy tọa độ quân cờ
    void Set_Position(int row, int col);      // Hàm sửa tọa độ quân cờ
    // Hàm thuần ảo: Ép các quân cờ con (Xe, Tượng...) phải tự định nghĩa cách đi riêng
    virtual bool Is_Valid_Move(int destRow, int destCol, const Board &grid) = 0;
};

#endif
