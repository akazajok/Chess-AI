#ifndef PIECE_H
#define PIECE_H

enum Color
{
    White,
    Black,
    None
};
enum Name
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
    int x, y; // tọa độ trên bàn cờ ( 0 -> 7 )
    Color color;
    Name name;

public:
    Piece(Name, Color, int x, int y);
    virtual ~Piece(); // xóa bộ nhớ

    Name Get_Name();            // Hàm xác định quân gì
    Color Get_Color();          // Hàm lấy màu quân cờ
    int Get_Piece_Coordinate(); // Hàm lấy tọa độ quân cờ

    // Hàm thuần ảo: Ép các quân cờ con (Xe, Tượng...) phải tự định nghĩa cách đi riêng
    // virtual bool IsValidMove(int destX, int destY, Piece *board[8][8]) = 0;
};

#endif
