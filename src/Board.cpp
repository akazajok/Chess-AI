#include "../include/Board.h"

void Board::Set_Up_Board(std::string &FEN)
{
    // Làm sạch bàn cờ cũ
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            grid[i][j] = nullptr;

    // tách chuỗi fen để xử lí
    std::stringstream fen(FEN);
    std::string piecePlacement;
    fen >> piecePlacement >> sideToMove >> castlingRights >> enPassantTarget >> halfmoveClock >> fullmoveNumber;

    // Parse CastlingRights//
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

    int row = 0, col = 0;                 // hàng và cột
    int lenBoard = piecePlacement.size(); // độ dài chuỗi FEN
    for (int i = 0; i < lenBoard; i++)
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
        char pieceLower = tolower(piecePlacement[i]);
        // ghim quân lên bàn cờ
        if (pieceTable.count(pieceLower))
        {
            grid[row][col] = pieceTable[pieceLower](color, row, col);
            // Kiểm tra có phải quân Vua không -> Cập nhật vị trí
            if (grid[row][col]->Get_Name() == Name::King)
            {
                bool isBlack = (grid[row][col]->Get_Color() == Color::Black);
                (isBlack ? rowKingBlack : rowKingWhite) = row;
                (isBlack ? colKingBlack : colKingWhite) = col;
            }
            col++;
        }
    }
}
// Hàm kiểm tra các quân cờ có được di chuyển hay không ( ngoại lệ && đúng luật )
bool Board::Can_Move(const int &startRow, const int &startCol, const int &destRow, const int &destCol)
{
    if (SpecialMove(startRow, startCol, destRow, destCol))
        return true;
    // kiểm tra biên
    if (destRow < 0 || destRow > 7 || destCol < 0 || destCol > 7)
        return false;
    // Nếu di chuyển đến vị trí cũ thì bỏ qua
    if (startRow == destRow && startCol == destCol)
        return false;

    Piece *movingPiece = Get_Piece_At(startRow, startCol);

    // Nếu là ô trống thì không di chuyển được
    if (!movingPiece)
        return false;

    Piece *targetPiece = Get_Piece_At(destRow, destCol);

    // Nếu ô đích là phe mình thì không di chuyển
    if (targetPiece && movingPiece->Get_Color() == targetPiece->Get_Color())
        return false;

    // Nếu đi sai luật thì không di chuyển
    if (!movingPiece->Is_Valid_Move(destRow, destCol, *this))
        return false;

    // Kiểm tra đường đi có bị chặn không ( Hậu, xe và tượng )
    Name nameMoving = movingPiece->Get_Name();
    if (nameMoving == Name::Queen || nameMoving == Name::Rook || nameMoving == Name::Bishop)
    {
        Piece *blocker = Get_Piece_On_Path(startRow, startCol, destRow, destCol);
        if (blocker && blocker != targetPiece)
            return false;
    }

    return true;
}

// Thực thi di chuyển quân cờ
void Board::Execute_Move(const int &startRow, const int &startCol, const int &destRow, const int &destCol)
{
    // Kiểm nước đặc biệt TRƯỚC KHI track movement
    if (SpecialMove(startRow, startCol, destRow, destCol))
    {
        ExecuteSpecialMove(startRow, startCol, destRow, destCol);
        return;
    }
    TrackPieceMovement(startRow, startCol);

    if (!Can_Move(startRow, startCol, destRow, destCol))
        return;

    //--------------------Hòa do 50 nước-----------------------
    Piece *movingPiece = Get_Piece_At(startRow, startCol);
    Piece *targetPiece = Get_Piece_At(destRow, destCol);

    // Nếu là nước đi của Tốt hoặc có ăn quân -> Reset clock
    if (movingPiece->Get_Name() == Name::Pawn || targetPiece != nullptr)
        halfmoveClock = 0;
    else
        halfmoveClock++;
    //-----------------------------------------------------------

    //--------------------Ăn tốt qua đường---------------------
    enPassantTarget = "-";
    if (movingPiece->Get_Name() == Name::Pawn && abs(destRow - startRow) == 2)
    {
        int result = (startRow + destRow) / 2; // hàng ở giữa ;
        enPassantTarget = convert_from_XY(result, destCol);
    }
    //---------------------------------------------------------

    fullmoveNumber++;

    // Rồi đi như bth
    Update_Position(startRow, startCol, destRow, destCol);
}

// Hàm cập nhật di chuyển quân cờ, ăn quân địch
void Board::Update_Position(const int &startRow, const int &startCol, const int &destRow, const int &destCol)
{
    // cập nhật quân cờ vào vị trí đích và xóa vị trí cũ
    grid[destRow][destCol] = std::move(grid[startRow][startCol]);
    grid[startRow][startCol] = nullptr;

    // cập nhật tọa độ mới cho quân cờ ( con trỏ )
    grid[destRow][destCol]->Set_Position(destRow, destCol);

    // Kiểm tra có phải quân Vua không -> Cập nhật vị trí
    if (grid[destRow][destCol]->Get_Name() == Name::King)
    {
        bool isBlack = (grid[destRow][destCol]->Get_Color() == Color::Black);
        (isBlack ? rowKingBlack : rowKingWhite) = destRow;
        (isBlack ? colKingBlack : colKingWhite) = destCol;
    }
}

//======================SPECIAL SECTION============================//
bool Board::SpecialMove(const int &startRow, const int &startCol, const int &destRow, const int &destCol)
{
    Piece *piece = Get_Piece_At(startRow, startCol);

    if (IsCastlingMove(startRow, startCol, destRow, destCol))
        return true;
    if (IsPromotion(startRow, startCol, destRow, destCol))
        return true;
    if (IsEnPassantMove(startRow, startCol, destRow, destCol))
        return true;
    return false;
}
void Board::ExecuteSpecialMove(const int &startRow, const int &startCol, const int &destRow, const int &destCol)
{

    // Bước xử lí phù hợp
    if (IsCastlingMove(startRow, startCol, destRow, destCol))
    {
        ExecuteCastling(startRow, startCol, destRow, destCol);
        return;
    }
    if (IsPromotion(startRow, startCol, destRow, destCol))
    {
        ExecutePromotion(startRow, startCol, destRow, destCol);
        return;
    }
    if (IsEnPassantMove(startRow, startCol, destRow, destCol))
    {
        ExecuteEnPassant(startRow, startCol, destRow, destCol);
        return;
    }
}
//=======================Promotion Func=======================//
bool Board::IsPromotion(const int &startRow, const int &startCol, const int &destRow, const int &destCol)
{
    Piece *piece = Get_Piece_At(startRow, startCol);
    if (!piece || piece->Get_Name() != Name::Pawn)
        return false; // check xem có phải quân thuờng không

    return (piece->Get_Color() == Color::White && destRow == 0) || // return true if này if nọ
           (piece->Get_Color() == Color::Black && destRow == 7);
}
void Board::ExecutePromotion(const int &startRow, const int &startCol, const int &destRow, const int &destCol)
{
    Piece *pawn = Get_Piece_At(startRow, startCol);
    Color pawncolor = pawn->Get_Color();
    // Update Position trước khi get promotion piece
    Update_Position(startRow, startCol, destRow, destCol);

    Name promotionpiece = GetPromotionChoice();

    switch (promotionpiece)
    {
    case Name::Queen:
        grid[destRow][destCol] = std::make_unique<Queen>(pawncolor, destRow, destCol); // check user choice rồi tạo một pointer mới
        break;
    case Name::Rook:
        grid[destRow][destCol] = std::make_unique<Rook>(pawncolor, destRow, destCol);
        break;
    case Name::Bishop:
        grid[destRow][destCol] = std::make_unique<Bishop>(pawncolor, destRow, destCol);
        break;
    case Name::Knight:
        grid[destRow][destCol] = std::make_unique<Knight>(pawncolor, destRow, destCol);
        break;
    }
}
Name Board::GetPromotionChoice()
{
    std::cout << "Phong Hậu! Chọn quân! Q/R/B/N" << std::endl;
    char choice;
    std::cin >> choice;
    choice = toupper(choice);

    switch (choice)
    {
    case 'Q':
        return Name::Queen;
    case 'R':
        return Name::Rook;
    case 'B':
        return Name::Bishop;
    case 'N':
        return Name::Knight;
    default:
        std::cout << "Sai Syntax, Auto Queen nha";
        return Name::Queen;
    }
}
//=======================Castling Func=======================//
bool Board::IsCastlingMove(const int &startRow, const int &startCol, const int &destRow, const int &destCol)
{
    Piece *piece = Get_Piece_At(startRow, startCol);

    if (!piece)
        return false;
    if (piece->Get_Name() == Name::King && // Nếu start là vua và sang ngang 2 ô
        destRow == startRow &&
        abs(destCol - startCol) == 2)

    {
        bool isKingside = (destCol > startCol);
        return CanCastle(piece->Get_Color(), isKingside);
    }
    return false;
}
bool Board::CanCastle(Color color, bool kingside)
{
    if ((color == Color::White && castlingFlags.whiteKing) ||
        (color == Color::Black && castlingFlags.blackKing))
        return false;

    bool rookMoved = (color == Color::White) ? (kingside ? castlingFlags.whiteRookKing : castlingFlags.whiteRookQueen) : (kingside ? castlingFlags.blackRookKing : castlingFlags.blackRookQueen);
    if (rookMoved)
        return false;

    int row = (color == Color::White) ? 7 : 0;
    return kingside ? (!Get_Piece_At(row, 5) && !Get_Piece_At(row, 6)) : (!Get_Piece_At(row, 1) && !Get_Piece_At(row, 2) && !Get_Piece_At(row, 3));
}
void Board::UpdateCastlingStat(Color color)
{
    if (color == Color::White)
    {
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
void Board::ExecuteCastling(const int &startRow, const int &startCol, const int &destRow, const int &destCol)
{
    bool isKingside = (destCol > startCol);

    Update_Position(startRow, startCol, destRow, destCol); // Move King

    int rookStartCol = isKingside ? 7 : 0;
    int rookDestCol = isKingside ? destCol - 1 : destCol + 1;
    Update_Position(destRow, rookStartCol, destRow, rookDestCol); // Move Rook

    UpdateCastlingStat(Get_Piece_At(destRow, destCol)->Get_Color());
}
void Board::ParseCastlingRights(const std::string &rights)
{
    // reset về true(để không nhập thành được)
    castlingFlags = {true, true, true, true, true, true};

    // parse kí hiệu trong fen - tối ưu logic
    for (char c : rights)
    {
        switch (c)
        {
        case 'K':
            castlingFlags.whiteRookKing = castlingFlags.whiteKing = false;
            break;
        case 'Q':
            castlingFlags.whiteRookQueen = castlingFlags.whiteKing = false;
            break;
        case 'k':
            castlingFlags.blackRookKing = castlingFlags.blackKing = false;
            break;
        case 'q':
            castlingFlags.blackRookQueen = castlingFlags.blackKing = false;
            break;
        }
    }
}
void Board::TrackPieceMovement(const int &startRow, const int &startCol)
{
    Piece *piece = Get_Piece_At(startRow, startCol);
    // Thêm null check
    if (!piece)
        return;
    if (piece->Get_Name() == Name::King)
    {
        if (piece->Get_Color() == Color::White)
            castlingFlags.whiteKing = true; // true = vua đã di chuyển
        else
            castlingFlags.blackKing = true;
    }
    else if (piece->Get_Name() == Name::Rook)
    {
        if (piece->Get_Color() == Color::White)
        {
            if (startRow == 7)
            {
                if (startCol == 0)
                    castlingFlags.whiteRookQueen = true;
                else if (startCol == 7)
                    castlingFlags.whiteRookKing = true;
            }
        }
        else
        {
            if (startRow == 0)
            {
                if (startCol == 0)
                    castlingFlags.blackRookQueen = true;
                else if (startCol == 7)
                    castlingFlags.blackRookKing = true;
            }
        }
    }
} //

//=======================================================//
//----------------------Ăn tốt qua đường-------------------
bool Board::IsEnPassantMove(const int &startRow, const int &startCol, const int &destRow, const int &destCol)
{
    Piece *pieceStart = Get_Piece_At(startRow, startCol);
    if (!pieceStart || pieceStart->Get_Name() != Name::Pawn)
        return false;

    if (enPassantTarget == "-")
        return false;

    // Phải là quân ăn chéo
    if (std::abs(startCol - destCol) != 1)
        return false;
    std::string result = convert_from_XY(destRow, destCol);
    if (enPassantTarget != result)
        return false;

    return true;
}
void Board::ExecuteEnPassant(const int &startRow, const int &startCol, const int &destRow, const int &destCol)
{
    Piece *pieceStart = Get_Piece_At(startRow, startCol);

    Update_Position(startRow, startCol, destRow, destCol);

    // xác định quân địch và ăn
    int step = (pieceStart->Get_Color() == Color::White) ? 1 : -1;
    grid[destRow + step][destCol] = nullptr;
    enPassantTarget = "-";
}

// lấy quân đang chặn đường || chiếu tướng
Piece *Board::Get_Piece_On_Path(const int &startRow, const int &startCol, const int &destRow, const int &destCol)
{
    // Xác định hướng di chuyển của xe hay tịnh
    int stepRow = (startRow == destRow) ? 0 : (destRow > startRow) ? 1
                                                                   : -1;
    int stepCol = (startCol == destCol) ? 0 : (destCol > startCol) ? 1
                                                                   : -1;
    int currR = startRow + stepRow, currC = startCol + stepCol;
    // Chạy vòng lặp kiểm tra các ô di chuyển
    while (true)
    {
        if (grid[currR][currC] != nullptr) // Thấy vật cản
            return Get_Piece_At(currR, currC);

        // đã tới ô đich thì dừng lại
        if (currR == destRow && currC == destCol)
            break;
        currR += stepRow;
        currC += stepCol;
    }
    return nullptr;
}

// lấy quân đang chiếu tướng
Piece *Board::Get_Checking_Piece(const int &rowKing, const int &colKing, const Color &colorKing)
{
    cntCheck = 0;                                  // bao nhiêu quân đang chiếu tướng
    int checkRow = 0, checkCol = 0;                // kiểm tra tọa độ
    Piece *pieceCheck = nullptr, *piece = nullptr; // quân cờ có thể chiếu tướng
    Name nameCheck;                                // tên quân cờ có thể chiếu tướng
    // Lấy tọa độ theo đường đi chéo của quân địch ( Hậu, vua, tượng, tốt )
    // Lấy theo chiều kim đồng hồ ( từ trái sang phải, trên xuống dưới )
    static const int directionRowDiagonal[] = {-1, -1, 1, 1}; // hướng di chuyển của đường đi chéo
    static const int directionColDiagonal[] = {-1, 1, 1, -1}; // hướng di chuyển của đường đi chéo
    static const int rowDiagonal[] = {0, 0, 7, 7};            // tọa độ hàng ở các góc ( 4 hướng )
    static const int colDiagonal[] = {0, 7, 7, 0};            // tọa độ cột ở các góc ( 4 hướng )
    int minStepDiagonal = -1;                                 // có thể đi chéo tối đa bao nhiêu
    for (int i = 0; i < 4; i++)
    {
        // Lấy tọa độ đường đi cần kiểm tra
        minStepDiagonal = std::min(abs(rowDiagonal[i] - rowKing), abs(colDiagonal[i] - colKing));
        if (minStepDiagonal == 0) // trùng || sai luật
            continue;
        checkRow = rowKing + minStepDiagonal * directionRowDiagonal[i];
        checkCol = colKing + minStepDiagonal * directionColDiagonal[i];

        // Quân cờ chiếu tướng
        piece = Get_Piece_On_Path(rowKing, colKing, checkRow, checkCol);
        if (!piece)
            continue;
        if (piece->Get_Color() == colorKing) // cùng phe bỏ qua
            continue;

        nameCheck = piece->Get_Name();
        if (nameCheck == Name::Queen || nameCheck == Name::Bishop)
        {
            cntCheck++;
            pieceCheck = piece;
        }
        else if (nameCheck == Name::King && piece->Is_Valid_Move(rowKing, colKing, *this))
        {
            cntCheck++;
            pieceCheck = piece;
        }
        else if (nameCheck == Name::Pawn && piece->Is_Valid_Move(rowKing, colKing, *this))
        {
            cntCheck++;
            pieceCheck = piece;
        }
        if (cntCheck > 1)

            return pieceCheck;
    }
    // Lấy tọa độ theo đường đi thẳng, ngang của quân địch ( Hậu, Vua, Xe )
    // Lấy theo đường thẳng ( trên xuống dưới ), rồi theo ngang ( trái sang phải )
    static const int rowStraight[] = {0, 7, -1, -1}; // -1 là lấy tọa độ hàng của vua
    static const int colStraight[] = {-1, -1, 0, 7}; // -1 là lấy tọa độ cột của vua
    for (int i = 0; i < 4; i++)
    {
        // tọa độ cần kiểm tra
        checkRow = (rowStraight[i] < 0) ? rowKing : rowStraight[i];
        checkCol = (colStraight[i] < 0) ? colKing : colStraight[i];

        // Quân cờ chiếu tướng
        piece = Get_Piece_On_Path(rowKing, colKing, checkRow, checkCol);
        if (!piece)
            continue;
        if (piece->Get_Color() == colorKing) // cùng phe bỏ qua
            continue;
        nameCheck = piece->Get_Name();
        if (nameCheck == Name::Queen || nameCheck == Name::Rook)
        {
            cntCheck++;
            pieceCheck = piece;
        }
        else if (nameCheck == Name::King && piece->Is_Valid_Move(rowKing, colKing, *this))
        {
            cntCheck++;
            pieceCheck = piece;
        }
        if (cntCheck > 1)
            return pieceCheck;
    }
    // Lấy tọa độ theo đường đi chữ L ( Quân mã )
    // Lấy theo chiều kim đồng hồ ( từ trái sang phải, trên xuống dưới )
    static const int directionRowHorse[] = {-1, -2, -2, -1, 1, 2, 2, 1}; // hướng di chuyển theo hàng quân mã
    static const int directionColHorse[] = {-2, -1, 1, 2, 2, 1, -1, -2}; // hướng di chuyển theo cột quân mã
    for (int i = 0; i < 8; i++)
    {
        checkRow = rowKing + directionRowHorse[i];
        checkCol = colKing + directionColHorse[i];

        // Quân cờ chiếu tướng
        piece = Get_Piece_At(checkRow, checkCol);
        if (!piece)
            continue;

        if (piece->Get_Color() == colorKing) // cùng phe bỏ qua
            continue;
        nameCheck = piece->Get_Name();
        if (nameCheck == Name::Knight)
        {
            cntCheck++;
            pieceCheck = piece;
        }
        if (cntCheck > 1)
            return pieceCheck;
    }
    return pieceCheck;
}

// có thể thoát chiếu tướng không
bool Board::Can_Escape_Check(const int &rowKing, const int &colKing, const Color &colorKing)
{
    int checkRow = 0, checkCol = 0;

    // Vua tự thoát chiếu
    static const int directionRowKing[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    static const int directionColKing[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    Piece *pieceKing = Get_Piece_At(rowKing, colKing);
    for (int i = 0; i < 8; i++)
    {
        // vua sẽ đi đến ô ( checkRow, CheckCol ) ;
        checkRow = rowKing + directionRowKing[i];
        checkCol = colKing + directionColKing[i];

        if (Is_Safe_Move(pieceKing, checkRow, checkCol, checkRow, checkCol, colorKing))
            return true;
    }

    // Ăn quân đang chiếu || Chặn đường quân chiếu
    // Di chuyển thì có bị chiếu tướng nữa không ???

    Piece *pieceCheck = Get_Checking_Piece(rowKing, colKing, colorKing);
    std::pair<int, int> posCheck = pieceCheck->Get_Position();
    // chiếu đôi thì không chạy được
    if (cntCheck > 1)
        return false;

    if (pieceCheck->Get_Name() == Name::Knight)
    {
        // tìm quân có thể ăn quân mã ( đang chiếu tướng ) && ktra có còn bị chiếu tướng không ?
        Piece *piece = Get_Checking_Piece(posCheck.first, posCheck.second, pieceCheck->Get_Color());
        if (piece && Is_Safe_Move(piece, posCheck.first, posCheck.second, rowKing, colKing, colorKing))
            return true;
    }
    int stepRow = (posCheck.first == rowKing) ? 0 : (rowKing > posCheck.first) ? 1
                                                                               : -1;
    int stepCol = (posCheck.second == colKing) ? 0 : (colKing > posCheck.second) ? 1
                                                                                 : -1;
    int currR = posCheck.first, currC = posCheck.second;
    // Chạy vòng lặp kiểm tra các ô di chuyển ( ăn quân địch || chặn đường để thoát chiếu bí)
    while (currR != rowKing || currC != colKing)
    {
        Piece *piece = Get_Checking_Piece(currR, currC, pieceCheck->Get_Color());
        if (piece && Is_Safe_Move(piece, currR, currC, rowKing, colKing, colorKing))
            return true;
        currR += stepRow;
        currC += stepCol;
    }
    return false;
}

// nước đi giả định ( đi thì có bị chiếu tướng không )
// return true là không bị chiếu tướng
bool Board::Is_Safe_Move(const Piece *piece, const int &destRow, const int &destCol, const int &rowKing, const int &colKing, const Color &colorKing)
{
    // Phải là nước đi hợp lệ thì mới kiểm tra
    std::pair<int, int> pos = piece->Get_Position();
    if (Can_Move(pos.first, pos.second, destRow, destCol))
    {
        // Di chuyển đến vị trí mới để kiểm tra
        std::unique_ptr<Piece> capturedPiece = std::move(grid[destRow][destCol]);
        Update_Position(pos.first, pos.second, destRow, destCol);

        Piece *checkingPiece = Get_Checking_Piece(rowKing, colKing, colorKing);
        bool canEscape = (checkingPiece == nullptr);

        // Trả về như cũ
        Update_Position(destRow, destCol, pos.first, pos.second);
        grid[destRow][destCol] = std::move(capturedPiece);

        return canEscape;
    }
}

//--------------------------------------------Hòa cờ---------------------------------------------
// Hòa do không đủ quân chiếu bí
bool Board::Is_Insufficient_Material() const
{
    // Đếm số lượng quân Tượng và quân Mã của phe Trắng
    int whiteBishops = 0, whiteKnights = 0;
    // Đếm số lượng quân Tượng và quân Mã của phe Đen
    int blackBishops = 0, blackKnights = 0;

    // whiteMinorPieces, blackMinorPieces: Tổng số "quân Tượng và Mã" (Minor Pieces) của mỗi bên.
    int whiteMinorPieces = 0;
    int blackMinorPieces = 0;

    // Lưu màu của ô mà quân Tượng đang đứng.
    // 0 - trắng, 1 - đen
    int whiteBishopSquareColor = -1;
    int blackBishopSquareColor = -1;

    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            Piece *p = Get_Piece_At(r, c);
            if (!p)
                continue;

            Name name = p->Get_Name();
            Color color = p->Get_Color();

            if (name == Name::Pawn || name == Name::Rook || name == Name::Queen)
                return false;

            if (name == Name::Bishop)
            {
                if (color == Color::White)
                {
                    whiteBishops++;
                    whiteMinorPieces++;
                    whiteBishopSquareColor = (r + c) % 2;
                }
                else
                {
                    blackBishops++;
                    blackMinorPieces++;
                    blackBishopSquareColor = (r + c) % 2;
                }
            }
            else if (name == Name::Knight)
            {
                if (color == Color::White)
                {
                    whiteKnights++;
                    whiteMinorPieces++;
                }
                else
                {
                    blackKnights++;
                    blackMinorPieces++;
                }
            }
        }
    }

    // 1. Vua đối Vua
    if (whiteMinorPieces == 0 && blackMinorPieces == 0)
        return true;

    // Một bên chỉ có Vua, bên kia có thêm đúng 1 Tượng hoặc 1 Mã.
    if ((whiteMinorPieces == 1 && blackMinorPieces == 0) ||
        (whiteMinorPieces == 0 && blackMinorPieces == 1))
        return true;

    // 3. Vua + Tượng đối Vua + Tượng (Cùng màu ô):
    if (whiteMinorPieces == 1 && blackMinorPieces == 1 && whiteBishops == 1 && blackBishops == 1)
    {
        if (whiteBishopSquareColor == blackBishopSquareColor)
            return true;
    }

    return false;
}

// Hòa do không còn nước đi hợp lệ
bool Board::Has_Legal_Moves(Color color)
{
    // Lấy tọa độ Vua của phe cần kiểm tra
    int rKing = (color == Color::White) ? rowKingWhite : rowKingBlack;
    int cKing = (color == Color::White) ? colKingWhite : colKingBlack;

    // Duyệt qua toàn bộ bàn cờ để tìm các quân cờ thuộc phe 'color'
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            Piece *p = Get_Piece_At(r, c);

            // Nếu ô có quân cờ và đúng màu phe mình
            if (p && p->Get_Color() == color)
            {
                // Thử di chuyển quân cờ này đến mọi ô (destR, destC) trên bàn cờ
                for (int destR = 0; destR < 8; destR++)
                {
                    for (int destC = 0; destC < 8; destC++)
                    {

                        // Nếu là quân Vua, tọa độ Vua mới sẽ là điểm đích (destR, destC)
                        int tempRKing = (p->Get_Name() == Name::King) ? destR : rKing;
                        int tempCKing = (p->Get_Name() == Name::King) ? destC : cKing;

                        // Kiểm tra nước đi này có đúng luật và an toàn (không làm Vua bị chiếu) không
                        if (Is_Safe_Move(p, destR, destC, tempRKing, tempCKing, color))
                        {
                            return true; // Chỉ cần 1 nước đi hợp lệ là chưa bị Stalemate
                        }
                    }
                }
            }
        }
    }
    return false; // Duyệt hết mà không có nước nào đi được -> Stalemate hoặc Checkmate
}

// Hòa do luật 50 nước
bool Board::Is_Draw_By_50_Moves()
{
    if (halfmoveClock >= 100)
        return true;
    return false;
}
//-------------------------------------------------------------------------------------------------

std::string Board::Get_Current_FEN() const
{
    std::string fen = "";
    for (int row = 0; row < 8; row++)
    {
        int cnt = 0;
        for (int col = 0; col < 8; col++)
        {
            if (grid[row][col] == nullptr)
                cnt++;
            else
            {
                if (cnt > 0)
                {
                    fen += std::to_string(cnt);
                    cnt = 0;
                }
                char piece;
                switch (grid[row][col]->Get_Name())
                {
                case Name::Pawn:
                    piece = 'P';
                    break;
                case Name::Rook:
                    piece = 'R';
                    break;
                case Name::Knight:
                    piece = 'N';
                    break;
                case Name::Bishop:
                    piece = 'B';
                    break;
                case Name::Queen:
                    piece = 'Q';
                    break;
                case Name::King:
                    piece = 'K';
                    break;
                default:
                    break;
                }
                if (grid[row][col]->Get_Color() == Color::Black)
                    piece = tolower(piece);
                fen += piece;
            }
        }
        if (cnt > 0)
            fen += std::to_string(cnt);
        if (row < 7)
            fen += '/';
    }

    // phe đang đến lượt
    fen += " ";
    fen += sideToMove;

    // quyền nhập thành
    fen += " ";
    std::string rights = "";
    // Theo logic trong code của bạn: false nghĩa là chưa di chuyển (còn quyền)
    if (!castlingFlags.whiteKing)
    {
        if (!castlingFlags.whiteRookKing)
            rights += 'K';
        if (!castlingFlags.whiteRookQueen)
            rights += 'Q';
    }
    if (!castlingFlags.blackKing)
    {
        if (!castlingFlags.blackRookKing)
            rights += 'k';
        if (!castlingFlags.blackRookQueen)
            rights += 'q';
    }
    fen += rights.empty() ? "-" : rights;

    // Bắt tốt qua đường
    fen += " ";
    fen += enPassantTarget.empty() ? "-" : enPassantTarget;

    // Hòa cờ do 50 nước
    fen += " ";
    fen += std::to_string(halfmoveClock);

    // Tổng số nước đi
    fen += " ";
    fen += std::to_string(fullmoveNumber);

    return fen;
}

// Hàm hiển thị để kiểm tra
void Board::Display()
{
    // 1. In chỉ số cột phía trên (a b c d e f g h)
    std::cout << "\n    a b c d e f g h" << std::endl;
    std::cout << "  +-----------------+" << std::endl;

    for (int i = 0; i < 8; i++)
    {
        // 2. In chỉ số hàng bên trái (8 -> 1 tương ứng với i từ 0 -> 7)
        std::cout << 8 - i << " | ";

        for (int j = 0; j < 8; j++)
        {
            Piece *p = Get_Piece_At(i, j); // Lấy quân cờ tại ô hiện tại

            if (p == nullptr)
            {
                // In dấu chấm cho ô trống để dễ nhìn hơn "--"
                std::cout << ". ";
            }
            else
            {
                char symbol = ' ';
                // Xác định ký tự đại diện dựa trên Name
                switch (p->Get_Name())
                {
                case Name::Pawn:
                    symbol = 'P';
                    break;
                case Name::Rook:
                    symbol = 'R';
                    break;
                case Name::Knight:
                    symbol = 'N';
                    break;
                case Name::Bishop:
                    symbol = 'B';
                    break;
                case Name::Queen:
                    symbol = 'Q';
                    break;
                case Name::King:
                    symbol = 'K';
                    break;
                default:
                    symbol = '?';
                    break;
                }

                // Nếu là quân Đen, chuyển thành chữ thường
                if (p->Get_Color() == Color::Black)
                {
                    symbol = (char)tolower(symbol);
                }

                std::cout << symbol << " ";
            }
        }

        // 3. In chỉ số hàng bên phải và khung dọc
        std::cout << "| " << 8 - i << std::endl;
    }

    // 4. In khung dưới và chỉ số cột phía dưới
    std::cout << "  +-----------------+" << std::endl;
    std::cout << "    a b c d e f g h\n"
              << std::endl;
}
