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
bool Board::Can_Move(int startRow, int startCol, int destRow, int destCol)
{
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
void Board::Execute_Move(int startRow, int startCol, int destRow, int destCol)
{
    // Kiểm tra đi có đúng luật && các điều kiện ngoại lệ
    if (Can_Move(startRow, startCol, destRow, destCol))
        Update_Position(startRow, startCol, destRow, destCol);
}

// Hàm cập nhật di chuyển quân cờ, ăn quân địch
void Board::Update_Position(int startRow, int startCol, int destRow, int destCol)
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

Piece *Board::Get_Piece_On_Path(int startRow, int startCol, int destRow, int destCol)
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

Piece *Board::Get_Checking_Piece(int rowKing, int colKing, Color colorKing)
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

// return 1 là có thể thoát chiếu tướng
bool Board::Can_Escape_Check(int rowKing, int colKing, Color colorKing)
{
    int checkRow = 0, checkCol = 0;

    // Vua tự thoát chiếu
    static const int directionRowKing[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    static const int directionColKing[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    for (int i = 0; i < 8; i++)
    {
        checkRow = rowKing + directionRowKing[i];
        checkCol = colKing + directionColKing[i];
        if (Can_Move(rowKing, colKing, checkRow, checkCol))
        {
            Piece *pieceCheck = Get_Checking_Piece(checkRow, checkCol, colorKing);
            if (!pieceCheck) // Vị trí không có quân nào chiếu
                return true;
        }
    }
    if (cntCheck > 1)
        return false;
    // Ăn quân đang chiếu || Chặn đường quân chiếu
    Piece *pieceCheck = Get_Checking_Piece(rowKing, colKing, colorKing);
    std::pair<int, int> posCheck = pieceCheck->Get_Position();
    // di chuyển từ quân chiếu tướng đến vua
    int stepRow = (posCheck.first == rowKing) ? 0 : (rowKing > posCheck.first) ? 1
                                                                               : -1;
    int stepCol = (posCheck.second == colKing) ? 0 : (colKing > posCheck.second) ? 1
                                                                                 : -1;
    int currR = posCheck.first, currC = posCheck.second;
    // Chạy vòng lặp kiểm tra các ô di chuyển
    while (currR != rowKing || currC != colKing)
    {
        Piece *piece = Get_Checking_Piece(currR, currC, pieceCheck->Get_Color());
        if (piece)
        {
            std::pair<int, int> pos = piece->Get_Position();
            Update_Position(pos.first, pos.second, currR, currC);
            if (!Get_Checking_Piece(rowKing, colKing, colorKing))
                return true;
        }
        currR += stepRow;
        currC += stepCol;
    }
    return false;
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
