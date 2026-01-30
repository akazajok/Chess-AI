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
    
    Piece* piece=Get_Piece_At(startRow, startCol);
    
    //Kiểm nước đặc biệt TRƯỚC KHI track movement
    if (SpecialMove(startRow, startCol, destRow, destCol)){
        ExecuteSpecialMove(startRow, startCol, destRow, destCol);
        return;
    }
    
    TrackPieceMovement(startRow, startCol);
    
    //Rồi đi như bth
    Update_Position(startRow, startCol, destRow, destCol);
}
//======================SPECIAL SECTION============================//
bool Board::SpecialMove(int startRow, int startCol, int destRow, int destCol){
    Piece* piece = Get_Piece_At(startRow, startCol);

    if (IsCastlingMove(startRow,startCol,destRow,destCol))
        return true;
    
    if (IsPromotion(startRow, startCol, destRow, destCol))
        return true;
    //Thêm Enpassant vào đây
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
    if (IsPromotion(startRow, startCol,destRow,destCol)){
        ExecutePromotion(startRow,startCol,destRow,destCol);
        return;
    }
    //Placeholder cho enpassant
    //
    //
}
//=======================Promotion Func=======================//
bool Board::IsPromotion(int startRow, int startCol, int destRow, int destCol){
    Piece* piece = Get_Piece_At(startRow, startCol);
        if (!piece || piece->Get_Name() != Name::Pawn) return false; //check xem có phải quân thuờng không

        return (piece->Get_Color() == Color::White && destRow == 0) || //return true if này if nọ
            (piece->Get_Color() == Color::Black && destRow == 7);
}

void Board::ExecutePromotion(int startRow, int startCol, int destRow, int destCol){
    Piece* pawn = Get_Piece_At(startRow,startCol);
    Color pawncolor = pawn->Get_Color();
    //Update Position trước khi get promotion piece
    Update_Position(startRow,startCol,destRow,destCol);

    Name promotionpiece = GetPromotionChoice();

    switch(promotionpiece){
        case Name::Queen : 
            grid[destRow][destCol] = std::make_unique<Queen>(pawncolor,destRow,destCol); //check user choice rồi tạo một pointer mới
            break;
        case Name::Rook :
            grid[destRow][destCol] = std::make_unique<Rook>(pawncolor,destRow,destCol);
            break;
        case Name::Bishop:
            grid[destRow][destCol] = std::make_unique<Bishop>(pawncolor,destRow,destCol);
            break;
        case Name::Knight:
            grid[destRow][destCol] = std::make_unique<Knight>(pawncolor,destRow,destCol);
            break;
    }
}

Name Board::GetPromotionChoice(){
    std::cout << "Phong Hậu! Chọn quân! Q/R/B/N"<<std::endl;
    char choice;
    std::cin >> choice;
    choice = toupper(choice);

    switch(choice) {
        case 'Q': return Name::Queen;
        case 'R': return Name::Rook;
        case 'B': return Name::Bishop;
        case 'N': return Name::Knight;
        default:
            std::cout<< "Sai Syntax, Auto Queen nha";
            return Name::Queen;
    }

}    
//=======================Castling Func=======================//
bool Board::IsCastlingMove(int startRow, int startCol, int destRow, int destCol){
    Piece* piece = Get_Piece_At(startRow,startCol);

    if (!piece) return false;
    if (piece->Get_Name() == Name::King && //Nếu start là vua và sang ngang 2 ô
        destRow == startRow &&
        abs(destCol-startCol)==2)

        {bool isKingside = (destCol>startCol);
        return CanCastle(piece->Get_Color(), isKingside);}
    return false;
}
bool Board::CanCastle(Color color, bool kingside){
    if((color == Color::White && castlingFlags.whiteKing) || 
       (color == Color::Black && castlingFlags.blackKing)) 
        return false;
    
    bool rookMoved = (color == Color::White) ? 
        (kingside ? castlingFlags.whiteRookKing : castlingFlags.whiteRookQueen) :
        (kingside ? castlingFlags.blackRookKing : castlingFlags.blackRookQueen);
    if(rookMoved) return false;
    
    int row = (color == Color::White) ? 7 : 0;
    return kingside ? 
        (!Get_Piece_At(row,5) && !Get_Piece_At(row,6)) :  
        (!Get_Piece_At(row,1) && !Get_Piece_At(row,2) && !Get_Piece_At(row,3)); 
}
void Board::UpdateCastlingStat(Color color){
    if (color == Color::White){
        castlingFlags.whiteKing = true;
        castlingFlags.whiteRookKing = true;
        castlingFlags.whiteRookQueen = true;
    }
    else
    {
        castlingFlags.blackKing = true;
        castlingFlags.blackRookKing = true;
        castlingFlags.blackRookQueen = true;
    }
}

void Board::ExecuteCastling(int startRow, int startCol, int destRow, int destCol){
    bool isKingside = (destCol > startCol);
    
    Update_Position(startRow, startCol, destRow, destCol); // Move King

    int rookStartCol = isKingside ? 7 : 0;
    int rookDestCol = isKingside ? destCol - 1 : destCol + 1;
    Update_Position(destRow, rookStartCol, destRow, rookDestCol); // Move Rook
    
    UpdateCastlingStat(Get_Piece_At(destRow, destCol)->Get_Color());
}

void Board::ParseCastlingRights(const std::string & rights){
    //reset về true(để không nhập thành được)
    castlingFlags = {true, true, true, true, true, true};
    
    //parse kí hiệu trong fen - tối ưu logic
    for (char c : rights){
        switch(c){
            case 'K': castlingFlags.whiteRookKing=castlingFlags.whiteKing=false; break;
            case 'Q': castlingFlags.whiteRookQueen=castlingFlags.whiteKing=false; break;
            case 'k': castlingFlags.blackRookKing=castlingFlags.blackKing=false; break;
            case 'q': castlingFlags.blackRookQueen=castlingFlags.blackKing=false; break;
        }
    }
}

void Board::TrackPieceMovement(int startRow, int startCol){
    Piece*piece=Get_Piece_At(startRow,startCol);
    // Thêm null check
    if (!piece) return;
    if(piece->Get_Name()==Name::King){
        if(piece->Get_Color()==Color::White) castlingFlags.whiteKing=true;//true = vua đã di chuyển
        else castlingFlags.blackKing=true;
    }
    else if(piece->Get_Name() == Name::Rook){
        if(piece->Get_Color() == Color::White){
            if(startRow == 7) {
                if(startCol == 0) castlingFlags.whiteRookQueen = true;
                else if(startCol == 7) castlingFlags.whiteRookKing = true;
            }
        } else {
            if(startRow == 0) { 
                if(startCol == 0) castlingFlags.blackRookQueen = true;
                else if(startCol == 7) castlingFlags.blackRookKing = true;
            }
        }
    }
}   //

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
