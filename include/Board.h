#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <functional>
#include <array>
#include <map>
#include <string>
#include <memory>
#include <sstream>
#include <cstdlib> // hàm abs()
#include <utility> // pair
#include <cmath>   // lấy phần dư kiểu float, double

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
    //=========Promotion Helper=========//
    bool IsPromotion(int startRow, int startCol, int destRow, int destCol);
    void ExecutePromotion(int startRow, int startCol, int destRow, int destCol);
    Name GetPromotionChoice(); // Prompt user input
    //=========Castling Tracker=========//
    struct CastlingFlags {
        bool whiteKing : 1;    
        bool blackKing : 1;      
        bool whiteRookKing : 1;  
        bool whiteRookQueen : 1; 
        bool blackRookKing : 1;  
        bool blackRookQueen : 1;
    } castlingFlags = {false}; 
    //=========Castling Helper==========//
    bool IsCastlingMove(int startRow, int startCol, int destRow, int destCol);
    void ExecuteCastling(int startRow, int startCol, int destRow, int destCol);
    bool CanCastle(Color color, bool kingside);
    void UpdateCastlingStat(Color color);
    void ParseCastlingRights(const std::string& rights);//Giải mã FEN của Castling
    void TrackPieceMovement(int startRow, int startCol);//Track đã di chuyển hay chưa, có thể recycle
    //==========Move History============//
    struct MoveRecord{
        int startRow, startCol, destRow,destCol;
        std::unique_ptr<Piece> capturedPiece;
        CastlingFlags previousCastlingState;
        bool WasSpecialMove;
        std::string FEN;

    };
    std::vector<MoveRecord> moveHistory;
    int currentIndex = -1;
    
public:
    int cntCheck = 0;               // bao nhiêu quân đang chiếu tướng
    int rowKingBlack, colKingBlack; // vị trí quân vua phe đen
    int rowKingWhite, colKingWhite; // vị trí quân vua phe trắng

    // hàm khởi tạo bàn cờ theo yêu cầu ( nhập dữ liệu chữ )
    void Set_Up_Board(std::string &FEN);
    void Display(); // Hàm kiểm tra
    // Hàm kiểm tra ngoại lệ && đúng luật
    bool Can_Move(int startRow, int startCol, int destRow, int destCol);
    // Hàm di chuyển quân cờ
    void Update_Position(int startRow, int startCol, int destRow, int destCol);
    // Thực thi di chuyển quân cờ
    void Execute_Move(int startRow, int startCol, int destRow, int destCol);
    // Lấy quân cờ chặn đường
    Piece *Get_Piece_On_Path(int startRow, int startCol, int destRow, int destCol);
    // Lấy quân cờ đang chiếu tướng
    Piece *Get_Checking_Piece(int rowKing, int colKing, Color colorKing);
    // Có thể thoát chiếu tướng không
    bool Can_Escape_Check(int rowKing, int colKing, Color colorKing);
    // Hàm lấy dữ liệu quân cờ
    Piece *Get_Piece_At(int row, int col) const
    {
        if (row < 0 || row > 7 || col < 0 || col > 7)
            return nullptr;
        return grid[row][col].get();
    }
    //=====Rollback và history=====//
    void SaveMoveToHistory(int startRow, int startCol, int destRow, int destCol);
    bool Undo(); //check trạng thái redo hay undo
    bool Redo();
    void ShowMoveHistory();
    std::string GetFen(int startRow, int startCol, int destRow, int destCol);
};

#endif
