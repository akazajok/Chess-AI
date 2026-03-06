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

    // Hàm lấy nước đi tốt nhất
    std::string getBestMove(std::string fen)
    {
        sendCommand("position fen " + fen);
        sendCommand("go movetime 1000"); // AI suy nghĩ trong 1 giây

        char buffer[4096];
        std::string output = "";
        DWORD read;

        // Đọc output cho đến khi thấy "bestmove"
        while (true)
        {
            if (ReadFile(hOutRead, buffer, sizeof(buffer) - 1, &read, NULL) && read > 0)
            {
                buffer[read] = '\0';
                output += buffer;
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