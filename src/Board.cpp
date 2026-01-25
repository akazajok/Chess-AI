#include "../include/Board.h"

void Board::Set_Up_Board(std::string board_test)
{
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

    for (int i = 0; i < board_test.size(); i++)
    {
        if (board_test[i] == ' ')
            break;
        if (board_test[i] == '/') // xuống hàng
        {
            row++;
            col = 0;
            continue;
        }
        if (board_test[i] >= '1' && board_test[i] <= '8') // số ô trống
        {
            col += (board_test[i] - '0');
            continue;
        }
        // phân biệt màu quân cờ
        Color color = (islower(board_test[i])) ? Color::Black : Color::White;
        // chuyển tất cả về chữ thường để kiểm tra, tránh kí tự đặc biệt
        char piece_lower = tolower(board_test[i]);
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
