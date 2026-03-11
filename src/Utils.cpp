#include "../include/Utils.h"

std::string convert_from_XY(const int &hang, const int &cot)
{
    char kitu_cot = cot + 97;
    char kitu_hang = '8' - hang;
    std::string ketqua = std::string(1, kitu_cot) + std::string(1, kitu_hang);
    return ketqua;
}

std::pair<int, int> convert_to_XY(std::string coord)
{
    char cot_chu = coord[0];
    char hang_chu = coord[1];
    int cot = cot_chu - 'a';
    int hang = '8' - hang_chu;
    return std::make_pair(hang, cot);
}

void updatePosition(std::string board[8][8], std::string move)
{
    std::string first_move = move.substr(0, 2);
    std::string last_move = move.substr(2, 2);
    std::pair<int, int> start = convert_to_XY(first_move);
    std::pair<int, int> end = convert_to_XY(last_move);
    std::string current = board[start.first][start.second];
    board[end.first][end.second] = current;
    board[start.first][start.second] = " .";
}
bool isValidMoveFormat(std::string move)
{
    if (move.length() == 4 &&
        move[0] >= 'a' && move[0] <= 'h' &&
        move[1] >= '1' && move[1] <= '8' &&
        move[2] >= 'a' && move[2] <= 'h' &&
        move[3] >= '1' && move[3] <= '8')
        return true;
    else
        return false;
}
// Trong utils.cpp
std::string to_lower(std::string s)
{
    for (char &c : s)
        c = std::tolower((unsigned char)c);
    return s;
}

std::string getFenFromImage(const std::string &imageUrl)
{
    // Trỏ chính xác đến python.exe trong môi trường ảo (.venv) của bạn
    // và trỏ đến file test.py
    std::string pythonPath = "F:\\Chess-AI\\.venv\\Scripts\\python.exe";
    std::string scriptPath = "F:\\Chess-AI\\chessboard2fen\\test.py";

    // Ghép lệnh command line
    std::string command = pythonPath + " " + scriptPath + " \"" + imageUrl + "\"";

    std::string result = "";
    char buffer[128];

    // Mở pipe để chạy lệnh và đọc luồng đầu ra (Standard Output)
    FILE *pipe = _popen(command.c_str(), "r");
    if (!pipe)
    {
        std::cerr << "Khong the khoi tao tien trinh Python!" << std::endl;
        return "";
    }

    // Đọc kết quả Python in ra
    while (fgets(buffer, sizeof(buffer), pipe) != NULL)
    {
        result += buffer;
    }
    _pclose(pipe);

    // Tìm chuỗi FEN từ kết quả trả về (Lọc theo tag FEN_RESULT: đã thiết lập ở Python)
    size_t pos = result.find("FEN_RESULT:");
    if (pos != std::string::npos)
    {
        // Cắt bỏ phần tag, chỉ lấy chuỗi FEN chuẩn
        std::string fen = result.substr(pos + 11);

        // Xóa các ký tự xuống dòng (newline) thừa ở cuối chuỗi nếu có
        fen.erase(fen.find_last_not_of(" \n\r\t") + 1);
        return fen;
    }

    std::cerr << "Loi tu Python: " << result << std::endl;
    return "";
}
