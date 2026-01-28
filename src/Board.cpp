#include "../include/Board.h"

void Board::Set_Up_Board(std::string &FEN)
{
    // tách chuỗi fen để xử lí
    std::stringstream fen(FEN);
    std::string piecePlacement;
    fen >> piecePlacement >> sideToMove >> castlingRights >> enPassantTarget >> halfmoveClock >> fullmoveNumber;

    //Parse CastlingRights//
    ParseCastlingRights(castlingRights);
    
    using PieceCreator = std::function<std::unique_ptr<Piece>(Color, int, int)>;
    std::map<char, PieceCreator> pieceTable = {
        {'p', [](Color color, int row, int col)
         { return std::make_unique<Pawn>(color, row, col); }}, // hàm lấy quân tốt
        {'r', [](Color color, int row, int col)
         { return std::make_unique<Rook>(color, row, col); }}, // hàm lấy quân xe
        {'b', [](Color color, int row, int col)
         { return std::make_unique<Bishop>(color, row, col); }}, // hàm lấy quân tịnh
        {'n', [](Color color, int row, int col)
         { return std::make_unique<Knight>(color, row, col); }}, // hàm lấy quân mã
        {'q', [](Color color, int row, int col)
         { return std::make_unique<Queen>(color, row, col); }}, // hàm lấy quân hậu
        {'k', [](Color color, int row, int col)
         { return std::make_unique<King>(color, row, col); }}}; // hàm lấy quân vua

    int row = 0, col = 0; // hàng và cột

    for (int i = 0; i < piecePlacement.size(); i++)
    {
        if (piecePlacement[i] == '/') // xuống hàng
        {
            row++;
            col = 0;
            continue;
        }
        if (piecePlacement[i] >= '1' && piecePlacement[i] <= '8') // số ô trống
        {
            col += (piecePlacement[i] - '0');
            continue;
        }
        // phân biệt màu quân cờ
        Color color = (islower(piecePlacement[i])) ? Color::Black : Color::White;
        // chuyển tất cả về chữ thường để kiểm tra, tránh kí tự đặc biệt
        char piece_lower = tolower(piecePlacement[i]);
        // ghim quân lên bàn cờ
        if (pieceTable.count(piece_lower))
        {
            grid[row][col] = pieceTable[piece_lower](color, row, col);
            col++;
        }
    }
}
// Hàm kiểm tra các quân cờ có được di chuyển hay không
bool Board::Can_Move(int startRow, int startCol, int destRow, int destCol)
{
    // Nếu là ô trống thì không di chuyển
    if (grid[startRow][startCol] == nullptr)
        return false;

    // Nếu vị trí không thay đổi thì không làm gì cả
    if (startRow == destRow && startCol == destCol)
        return false;

    // đích đến không hợp lệ, vượt biên
    if (destRow < 0 || destRow > 7 || destCol < 0 || destCol > 7)
        return false;

    // kiểm tra đích đến là ô trống, phe mình hay là địch
    if (grid[destRow][destCol] != nullptr)
    {
        Color color_start = grid[startRow][startCol]->Get_Color();
        Color color_dest = grid[destRow][destCol]->Get_Color();
        if (color_start == color_dest)
            return false; // phe mình thì không di chuyển
    }
    // Kiểm tra logic di chuyển của quân cờ
    return grid[startRow][startCol]->Is_Valid_Move(destRow, destCol, *this);
}

// di chuyển quân cờ
void Board::Execute_Move(int startRow, int startCol, int destRow, int destCol)
{
    if (!Can_Move(startRow, startCol, destRow, destCol))
        return;
    
    //Track Movement trước mỗi nước đi
    TrackPieceMovement(startRow, startCol);
    Piece* piece=Get_Piece_At(startRow, startCol);
    //Kiểm nước đặc biệt
    if (SpecialMove(startRow, startCol, destRow, destCol)){
        ExecuteSpecialMove(startRow, startCol, destRow, destCol);
        return;
    }
    //Rồi đi như bth
    Update_Position(startRow, startCol, destRow, destCol);
}
//======================SPECIAL SECTION============================//
bool Board::SpecialMove(int startRow, int startCol, int destRow, int destCol){
    Piece* piece = Get_Piece_At(startRow, startCol);

    if (IsCastlingMove(startRow,startCol,destRow,destCol))
        return true;
    //Thêm Enpassant với promotion vào đây
    //
    //
    return false;
}
void Board::ExecuteSpecialMove(int startRow, int startCol, int destRow, int destCol){
    
    //Bước xử lí phù hợp
    if (IsCastlingMove(startRow, startCol, destRow, destCol)){
        ExecuteCastling(startRow,startCol,destRow,destCol);
        return;
    }
    //Placeholder cho enpassant với phong hậu
    //
    //
}
//===========================================================//

//=======================Castling Func=======================//
bool Board::IsCastlingMove(int startRow, int startCol, int destRow, int destCol){
    Piece* piece = Get_Piece_At(startRow,startCol);
    if (piece->Get_Name() == Name::King && //Nếu start là vua và sang ngang 2 ô
        destRow == startRow &&
        abs(destCol-startCol)==2)

        {bool isKingside = (destCol>startCol);
        return CanCastle(piece->Get_Color(), isKingside);}
    return false;
}
bool Board::CanCastle(Color color, bool kingside){
    //Check King di chuyển chưa
    if(color==Color::White && whiteKing) return false;
    if(color==Color::Black && blackKing) return false;
    //Check Rook di chuyển chưa
    if (color == Color::White){
        if(kingside&&whiteRookKing) return false;
        if(!kingside&&whiteRookQueen) return false;
    }
    else{
        if(kingside&&blackRookKing) return false;
        if(!kingside&&blackRookQueen) return false;
    }
    //Check đường đi trống không
    int kingrow = (color==Color::White)?7:0;
    if (kingside){
        if(Get_Piece_At(kingrow,5)!=nullptr|| //kingside sẽ check f1-g1 hoặc f8-g8
            Get_Piece_At(kingrow,6)!=nullptr)
            return false;
    }
    else{
        if(Get_Piece_At(kingrow,1)!=nullptr||//queenside sẽ check b1-c1-d1 hoặc b8-c8-d8
            Get_Piece_At(kingrow,2)!=nullptr||
            Get_Piece_At(kingrow,3)!=nullptr)
            return false;
    }


    return true;
}
void Board::UpdateCastlingStat(Color color){
    if (color == Color::White){
        whiteKing = true;
        whiteRookKing = true;
        whiteRookQueen = true;
    }
    else
    {
        blackKing = true;
        blackRookKing = true;
        blackRookQueen = true;
    }
}

void Board::ExecuteCastling(int startRow, int startCol, int destRow, int destCol){
    Piece* king=Get_Piece_At(startRow,startCol);
    Color color=king->Get_Color();
    bool isKingside = (destCol>startCol);

    //Di Chuyển Vua
    Update_Position(startRow, startCol, destRow, destCol);
    
    //Di Chuyển Xe
    int rookStartCol = isKingside ? 7:0;
    int rookDestCol = isKingside ? destCol -1:destCol+1;   //DestCol là của vua nhá, đừng nhìn nhầm 
    Update_Position(startRow,rookStartCol,destRow,rookDestCol);

    //Update Stat
    UpdateCastlingStat(color);
}

void Board::ParseCastlingRights(const std::string & rights){
    //reset về true(để không nhập thành được)
    whiteKing=blackKing=whiteRookKing=whiteRookQueen=blackRookKing=blackRookQueen=true;
    //parse kí hiệu trong fen
    for (char c :rights){
        switch(c){
            case 'K': whiteRookKing=whiteKing=false; break;
            case 'Q': whiteRookQueen=whiteKing=false; break;
            case 'k': blackRookKing=blackKing=false; break;
            case 'q': blackRookQueen=blackKing=false; break;
        }
    }
}

void Board::TrackPieceMovement(int startRow, int startCol){
    Piece*piece=Get_Piece_At(startRow,startCol);
    if(piece->Get_Name()==Name::King){
        if(piece->Get_Color()==Color::White) whiteKing=true;//true = vua đã di chuyển
        else blackKing=true;
    }
    //Check start pos của rook để phân loại xe bên nào
    else if(piece->Get_Name()==Name::Rook){
        if(piece->Get_Color()==Color::White){
            if (startRow==7 && startCol==0) whiteRookQueen=true;//true = xe đã di chuyển
            if (startRow==7 && startCol==7) whiteRookKing = true;}
        else {
            if (startRow == 0 && startCol == 0) blackRookQueen = true; 
            if (startRow == 0 && startCol == 7) blackRookKing = true;  
        }
    }
}   

//=======================================================//

// Hàm cập nhật di chuyển quân cờ, ăn quân địch
void Board::Update_Position(int startRow, int startCol, int destRow, int destCol)
{
    // cập nhật quân cờ vào vị trí đích và xóa vị trí cũ
    grid[destRow][destCol] = std::move(grid[startRow][startCol]);
    grid[startRow][startCol] = nullptr;

    // cập nhật tọa độ mới cho quân cờ ( con trỏ )
    grid[destRow][destCol]->Set_Position(destRow, destCol);
}

// 4. Hàm hiển thị để kiểm tra
void Board::Display()
{
    std::cout << "  0 1 2 3 4 5 6 7" << '\n';
    for (int i = 0; i < 8; i++)
    {
        std::cout << i << " ";
        for (int j = 0; j < 8; j++)
        {
            if (grid[i][j] == nullptr)
            {
                std::cout << "--";
            }
            else
            {
                // Ở đây bạn có thể dùng GetName() để quyết định in chữ gì
                if (grid[i][j]->Get_Name() == Name::Pawn)
                    std::cout << (grid[i][j]->Get_Color() == Color::White ? "P " : "p ");
                if (grid[i][j]->Get_Name() == Name::Rook)
                    std::cout << (grid[i][j]->Get_Color() == Color::White ? "R " : "r ");
                if (grid[i][j]->Get_Name() == Name::Bishop)
                    std::cout << (grid[i][j]->Get_Color() == Color::White ? "B " : "b ");
                if (grid[i][j]->Get_Name() == Name::Knight)
                    std::cout << (grid[i][j]->Get_Color() == Color::White ? "N " : "n ");
                if (grid[i][j]->Get_Name() == Name::Queen)
                    std::cout << (grid[i][j]->Get_Color() == Color::White ? "Q " : "q ");
                if (grid[i][j]->Get_Name() == Name::King)
                    std::cout << (grid[i][j]->Get_Color() == Color::White ? "K " : "k ");
                // ... tương tự cho các quân khác
            }
        }
        std::cout << '\n';
    }
}
