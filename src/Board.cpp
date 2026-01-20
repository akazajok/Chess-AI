#include "../include/Board.h"

Board::Board()
{
    for (int i = 0; i <= 7; i++)
        for (int j = 0; j <= 7; j++)
            grid[i][j] = nullptr;
}

Board::~Board()
{
    for (int i = 0; i <= 7; i++)
        for (int j = 0; j <= 7; j++)
            if (grid[i][j] != nullptr)
                delete grid[i][j];
}

void Board::Set_Up_Board(std::string board_test)
{
    using PieceCreator = std::function<Piece *(Color, int, int)>;
    std::map<char, PieceCreator> chess = {
        {'p', [](Color color, int row, int col) // hàm lấy quân tốt
         { return new Pawn(color, row, col); }},
        {'r', [](Color color, int row, int col) // hàm lấy quân xe
         { return new Rook(color, row, col); }},
        {'b', [](Color color, int row, int col) // hàm lấy quân tịnh
         { return new Bishop(color, row, col); }},
        {'n', [](Color color, int row, int col) // hàm lấy quân mã
         { return new Knight(color, row, col); }},
        {'q', [](Color color, int row, int col) // hàm lấy quân hậu
         { return new Queen(color, row, col); }},
        {'k', [](Color color, int row, int col) // hàm lấy quân vua
         { return new King(color, row, col); }}};
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
            col += int(board_test[i] - 48);
            continue;
        }
        // phân biệt màu quân cờ
        Color color = (islower(board_test[i])) ? Color::Black : Color::White;
        // chuyển tất cả về chữ thường để kiểm tra, tránh kí tự đặc biệt
        char piece_lower = tolower(board_test[i]);
        // ghim quân lên bàn cờ
        if (chess.count(piece_lower))
        {
            grid[row][col] = chess[piece_lower](color, row, col);
            col++;
        }
    }
}

void Board::Update_Position(int startRow, int startCol, int endRow, int endCol)
{
    // Nếu vị trí không thay đổi thì không làm gì cả
    if (startRow == endRow && startCol == endCol)
        return;

    if (grid[endRow][endCol] != nullptr)
    {
        // xóa bộ nhớ quân cờ cũ ( đã bị ăn ) -> loại bỏ
        delete grid[endRow][endCol];
        grid[endRow][endCol] = nullptr;
    }

    // cập nhật vị trí mới trên bàn cờ và xóa vị trí cũ
    grid[endRow][endCol] = grid[startRow][startCol];
    grid[startRow][startCol] = nullptr;

    // tránh trường hợp start không có quân
    if (grid[endRow][endCol] != nullptr)
        grid[endRow][endCol]->Set_Position(endRow, endCol); // cập nhật vị trí mới cho con trỏ
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
