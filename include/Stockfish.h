#ifndef STOCKFISH_H
#define STOCKFISH_H

#include <windows.h>
#include <iostream>
#include <string>

class Stockfish
{
private:
    HANDLE hInRead, hInWrite;   // Luồng ghi lệnh vào Stockfish
    HANDLE hOutRead, hOutWrite; // Luồng đọc kết quả từ Stockfish

public:
    Stockfish(std::string path)
    {
        SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, TRUE};
        CreatePipe(&hOutRead, &hOutWrite, &sa, 0); // Pipe đầu ra
        CreatePipe(&hInRead, &hInWrite, &sa, 0);   // Pipe đầu vào

        STARTUPINFOA si = {sizeof(si)};
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdOutput = hOutWrite;
        si.hStdError = hOutWrite;
        si.hStdInput = hInRead;

        PROCESS_INFORMATION pi;
        // Khởi tạo tiến trình Stockfish
        if (!CreateProcessA(NULL, (char *)path.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
        {
            std::cerr << "Loi: Khong tim thay file Stockfish tai: " << path << std::endl;
        }
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    // Hàm gửi lệnh UCI
    void sendCommand(std::string command)
    {
        command += "\n";
        DWORD written;
        WriteFile(hInWrite, command.c_str(), command.length(), &written, NULL);
    }

    // Thêm biến này để lưu điểm đánh giá
    std::string lastEvaluation = "0.0";
    // Hàm lấy nước đi tốt nhất && lấy điểm bàn cờ
    std::string getBestMove(std::string fen)
    {
        // 1. Xác định phe nào đang đi bằng cách đọc ký tự lượt đi trong chuỗi FEN
        // FEN luôn có cấu trúc: [vị_trí_quân] [lượt_đi_w_hoặc_b] [nhập_thành]...
        bool isWhiteToMove = true;
        size_t spacePos = fen.find(' ');
        if (spacePos != std::string::npos && spacePos + 1 < fen.length())
        {
            if (fen[spacePos + 1] == 'b')
            {
                isWhiteToMove = false;
            }
        }

        sendCommand("position fen " + fen);
        sendCommand("go movetime 1000"); // AI suy nghĩ trong 1 giây

        char buffer[4096];
        std::string output = "";
        DWORD read;

        while (true)
        {
            if (ReadFile(hOutRead, buffer, sizeof(buffer) - 1, &read, NULL) && read > 0)
            {
                buffer[read] = '\0';
                output += buffer;

                // --- BÓC TÁCH ĐIỂM SỐ THEO CHUẨN QUỐC TẾ ---
                size_t cpPos = output.rfind("score cp ");
                if (cpPos != std::string::npos)
                {
                    size_t endPos = output.find(' ', cpPos + 9);
                    if (endPos != std::string::npos)
                    {
                        try
                        {
                            int cp = std::stoi(output.substr(cpPos + 9, endPos - (cpPos + 9)));

                            // ĐẢO DẤU NẾU LÀ PHE ĐEN ĐANG ĐI
                            if (!isWhiteToMove)
                                cp = -cp;

                            float score = cp / 100.0f;
                            lastEvaluation = (score > 0 ? "+" : "") + std::to_string(score);

                            // Xóa số 0 thừa phía sau
                            lastEvaluation.erase(lastEvaluation.find_last_not_of('0') + 1, std::string::npos);
                            if (lastEvaluation.back() == '.')
                                lastEvaluation.pop_back();
                        }
                        catch (...)
                        {
                        }
                    }
                }

                size_t matePos = output.rfind("score mate ");
                if (matePos != std::string::npos)
                {
                    size_t endPos = output.find(' ', matePos + 11);
                    if (endPos != std::string::npos)
                    {
                        try
                        {
                            int mateIn = std::stoi(output.substr(matePos + 11, endPos - (matePos + 11)));

                            // ĐẢO DẤU MATE NẾU LÀ PHE ĐEN ĐANG ĐI
                            if (!isWhiteToMove)
                                mateIn = -mateIn;

                            lastEvaluation = (mateIn > 0 ? "+M" : "-M") + std::to_string(std::abs(mateIn));
                        }
                        catch (...)
                        {
                        }
                    }
                }
                // ----------------------------------------

                size_t pos = output.find("bestmove ");
                if (pos != std::string::npos)
                {
                    return output.substr(pos + 9, 4); // Cắt lấy chuỗi tọa độ (vd: e2e4)
                }
            }
        }
    }

    // Hàm cài đặt độ khó cho AI
    void setSkillLevel(int level)
    {
        // level phải nằm trong khoảng từ 0 (dễ nhất) đến 20 (khó nhất)
        if (level < 0)
            level = 0;
        if (level > 20)
            level = 20;

        // Gửi lệnh setoption của chuẩn UCI
        sendCommand("setoption name Skill Level value " + std::to_string(level));
    }

    ~Stockfish()
    {
        sendCommand("quit");
        CloseHandle(hInWrite);
        CloseHandle(hInRead);
        CloseHandle(hOutWrite);
        CloseHandle(hOutRead);
    }
};

#endif