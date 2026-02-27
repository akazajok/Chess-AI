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
#include "../include/Utils.h"

class gameManager;

#include "../include/Pawn.h"
#include "../include/Rook.h"
#include "../include/Bishop.h"
#include "../include/Knight.h"
#include "../include/Queen.h"
#include "../include/King.h"

using BoardGrid = std::array<std::array<std::unique_ptr<Piece>, 8>, 8>;

class Board
{
    friend class gameManager;

private:
    BoardGrid grid;
    char sideToMove;                          // Ai là người đi
    std::string castlingRights;               // Quyền nhập thành ( vua, xe chưa di chuyển )
    std::string enPassantTarget;              // Bắt tốt qua đường
    int halfmoveClock;                        // Luật 50 nước
    int fullmoveNumber;                       // Số lượt đi của ván đấu
    int cntCheck = 0;                         // bao nhiêu quân đang chiếu tướng
    int rowKingBlack = -1, colKingBlack = -1; // vị trí quân vua phe đen
    int rowKingWhite = -1, colKingWhite = -1; // vị trí quân vua phe trắng

    //==========SPECIAL MOVE LOL========//
    bool SpecialMove(const int &startRow, const int &startCol, const int &destRow, const int &destCol);        // Check nước đặc biệt
    void ExecuteSpecialMove(const int &startRow, const int &startCol, const int &destRow, const int &destCol); // Như cái tên
    //=========Promotion Helper=========//
    bool IsPromotion(const int &startRow, const int &startCol, const int &destRow, const int &destCol);
    void ExecutePromotion(const int &startRow, const int &startCol, const int &destRow, const int &destCol);
    Name GetPromotionChoice(); // Prompt user input
    //=========Castling Tracker=========//
    struct CastlingFlags
    {
        bool whiteKing : 1;
        bool blackKing : 1;
        bool whiteRookKing : 1;
        bool whiteRookQueen : 1;
        bool blackRookKing : 1;
        bool blackRookQueen : 1;
    } castlingFlags = {false};
    //=========Castling Helper==========//
    bool IsCastlingMove(const int &startRow, const int &startCol, const int &destRow, const int &destCol);
    void ExecuteCastling(const int &startRow, const int &startCol, const int &destRow, const int &destCol);
    bool CanCastle(Color color, bool kingside);
    void UpdateCastlingStat(Color color);
    void ParseCastlingRights(const std::string &rights);               // Giải mã FEN của Castling
    void TrackPieceMovement(const int &startRow, const int &startCol); // Track đã di chuyển hay chưa, có thể recycle
    // ăn tốt qua đường
    bool IsEnPassantMove(const int &startRow, const int &startCol, const int &destRow, const int &destCol);
    void ExecuteEnPassant(const int &startRow, const int &startCol, const int &destRow, const int &destCol);

public:
    // hàm khởi tạo bàn cờ theo yêu cầu ( nhập dữ liệu chữ )
    void Set_Up_Board(std::string &FEN);
    // Hàm kiểm tra
    void Display();

    // Hàm kiểm tra ngoại lệ && đúng luật
    bool Can_Move(const int &startRow, const int &startCol, const int &destRow, const int &destCol);

    // Hàm di chuyển quân cờ
    void Update_Position(const int &startRow, const int &startCol, const int &destRow, const int &destCol);

    // Thực thi di chuyển quân cờ
    void Execute_Move(const int &startRow, const int &startCol, const int &destRow, const int &destCol);

    // Lấy quân cờ chặn đường
    Piece *Get_Piece_On_Path(const int &startRow, const int &startCol, const int &destRow, const int &destCol);

    // Lấy quân cờ đang chiếu tướng
    Piece *Get_Checking_Piece(const int &rowKing, const int &colKing, const Color &colorKing);

    // Có thể thoát chiếu tướng không
    bool Can_Escape_Check(const int &rowKing, const int &colKing, const Color &colorKing);

    // Di chuyển thì vua có bị chiếu tướng không ???
    bool Is_Safe_Move(const Piece *pieceCheck, const int &destRow, const int &destCol, const int &rowKing, const int &colKing, const Color &colorking);

    // Hòa do không đủ quân chiếu bí
    bool Is_Insufficient_Material() const;

    // Kiểm tra xem phe (color) còn nước đi hợp lệ nào không
    bool Has_Legal_Moves(Color color);

    // Hòa do luật 50 nước
    bool Is_Draw_By_50_Moves();

    // Hàm lấy dữ liệu quân cờ
    Piece *Get_Piece_At(int row, int col) const
    {
        if (row < 0 || row > 7 || col < 0 || col > 7)
            return nullptr;
        return grid[row][col].get();
    }
};

#endif
