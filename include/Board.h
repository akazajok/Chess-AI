#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <functional>
#include <array>
#include <map>
#include <string>
#include <memory>
#include <sstream>

#include "../include/Piece.h"

#include "../include/Pawn.h"
#include "../include/Rook.h"
#include "../include/Bishop.h"
#include "../include/Knight.h"
#include "../include/Queen.h"
#include "../include/King.h"

using BoardGrid = std::array<std::array<std::unique_ptr<Piece>, 8>, 8>;
class Board
{
private:
    BoardGrid grid;
    char sideToMove;             // Ai là người đi tiếp theo?
    std::string castlingRights;      // Quyền nhập thành ( vua, xe chưa di chuyển )
    std::string enPassantTarget;     // Bắt tốt qua đường
    int halfmoveClock;           // Luật 50 nước
    int fullmoveNumber;         // Số lượt đi của ván đấu
    //==========SPECIAL MOVE LOL========//
    bool SpecialMove(int startRow, int startCol, int destRow, int destCol); //Check nước đặc biệt
    void ExecuteSpecialMove(int startRow, int startCol, int destRow, int destCol);//Như cái tên
    //=========Castling Tracker=========//
    bool whiteKing = false;//false = chưa di chuyển
    bool blackKing = false;
    bool blackRockKing = false;
    bool blackRockQueen = false;
    bool whiteRockKing = false;
    bool whiteRockQueen = false;
    //=========Castling Helper==========//
    bool IsCastlingMove(int startRow, int startCol, int destRow, int destCol);
    void ExecuteCastling(int startRow, int startCol, int destRow, int destCol);
    bool CanCastle(Color color, bool kingside);
    void UpdateCastlingStat(Color color);
    void ParseCastlingRights(const std::string& rights);//Giải mã FEN của Castling
    void TrackPieceMovement(int startRow, int startCol);//Track đã di chuyển hay chưa, có thể recycle

public:
    // hàm khởi tạo bàn cờ theo yêu cầu ( nhập dữ liệu chữ )
    void Set_Up_Board(std::string &FEN);
    void Display(); // Hàm kiểm tra
    // Hàm kiểm tra quân cờ di chuyển có đúng luật không
    bool Can_Move(int startRow, int startCol, int destRow, int destCol);
    // Hàm di chuyển quân cờ
    void Update_Position(int startRow, int startCol, int destRow, int destCol);
    // Hàm thực thi lệnh kiểm tra và di chuyển
    void Execute_Move(int startRow, int startCol, int destRow, int destCol);
    // Hàm lấy dữ liệu quân cờ
    Piece *Get_Piece_At(int row, int col) const
    {
        if (row < 0 || row > 7 || col < 0 || col > 7)
            return nullptr;
        return grid[row][col].get();
    }
};

#endif
