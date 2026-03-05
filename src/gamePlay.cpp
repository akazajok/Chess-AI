#include "../include/gamePlay.h"
#include "../include/Stockfish.h"

gameManager::gameManager() : rowKing(-1), colKing(-1), colorKing(Color::None) {}

void gameManager::Init_Game(std::string FEN, GameMode mode)
{
    chessGame.Set_Up_Board(FEN); // Khởi tạo quân cờ lên grid
    currentMode = mode;
}

bool gameManager::Is_Valid_Input(const std::string &moveStr)
{
    // 1. Kiểm tra định dạng chuỗi nhập vào (vd: e2e4)
    if (!isValidMoveFormat(moveStr))
    {
        std::cout << "=> Sai dinh dang! Vui long nhap lai (vi du: e2e4).\n";
        return false;
    }

    std::pair<int, int> start = convert_to_XY(moveStr.substr(0, 2));
    std::pair<int, int> dest = convert_to_XY(moveStr.substr(2, 2));

    Piece *p = chessGame.Get_Piece_At(start.first, start.second);

    // 2. Kiểm tra ô bắt đầu có quân và có đúng lượt hay không
    if (!p)
    {
        std::cout << "=> O bat dau khong co quan co!\n";
        return false;
    }

    char currentTurn = chessGame.sideToMove;
    if ((currentTurn == 'w' && p->Get_Color() != Color::White) ||
        (currentTurn == 'b' && p->Get_Color() != Color::Black))
    {
        std::cout << "=> Khong phai luot cua ban! (" << (currentTurn == 'w' ? "Trang" : "Den") << " dang di)\n";
        return false;
    }

    // 3. Kiểm tra luật di chuyển của quân cờ đó
    if (!chessGame.Can_Move(start.first, start.second, dest.first, dest.second))
    {
        std::cout << "=> Nuoc di khong dung luat!\n";
        return false;
    }

    // 4. Kiểm tra xem sau nước đi Vua có bị chiếu tướng không
    int tempRowKing = (p->Get_Name() == Name::King) ? dest.first : rowKing;
    int tempColKing = (p->Get_Name() == Name::King) ? dest.second : colKing;

    if (!chessGame.Is_Safe_Move(p, dest.first, dest.second, tempRowKing, tempColKing, colorKing))
    {
        std::cout << "=> Nuoc di nguy hiem! Vua se bi chieu tuong.\n";
        return false;
    }

    return true;
}

void gameManager::Game_Turn()
{

    // Khởi tạo AI (đảm bảo đường dẫn tới file .exe chính xác)
    Stockfish ai("stockfish-windows-x86-64-avx2.exe");

    while (true)
    {
        // Cập nhật tọa độ Vua của phe hiện tại từ Board
        if (chessGame.sideToMove == 'w')
        {
            rowKing = chessGame.rowKingWhite;
            colKing = chessGame.colKingWhite;
        }
        else
        {
            rowKing = chessGame.rowKingBlack;
            colKing = chessGame.colKingBlack;
        }

        Piece *king = chessGame.Get_Piece_At(rowKing, colKing);
        if (!king || king->Get_Name() != Name::King)
        {
            std::string loser = (chessGame.sideToMove == 'w') ? "TRANG" : "DEN";
            std::string winner = (chessGame.sideToMove == 'w') ? "DEN" : "TRANG";
            std::cout << ">>> VUA " << loser << " DA BI AN! "
                      << "PHE " << winner << " THANG! <<<\n";
            break;
        }
        colorKing = king->Get_Color();

        chessGame.Display(); // Hiển thị bàn cờ ra console
        std::cout << chessGame.enPassantTarget << " " << chessGame.fullmoveNumber << '\n';

        std::cout << "\nLUOT CUA PHE: " << (chessGame.sideToMove == 'w' ? "TRANG [W]" : "DEN [B]") << "\n";

        // Kiểm tra hòa do thiếu quân chiếu bí
        if (chessGame.Is_Insufficient_Material())
        {
            std::cout << ">>> HOA CO! Khong du quan de chieu bi. <<<\n";
            break;
        }

        // Hòa do luật 50 nước
        if (chessGame.Is_Draw_By_50_Moves())
        {
            std::cout << "Hòa do luật 50 nước";
            break;
        }

        // Kiểm tra tình trạng chiếu tướng
        if (chessGame.Get_Checking_Piece(rowKing, colKing, colorKing))
        {
            std::cout << "!!! CANH BAO: Vua dang bi chieu tuong !!!\n";
            if (!chessGame.Can_Escape_Check(rowKing, colKing, colorKing))
            {
                std::cout << ">>> CHIEU BI! Tro choi ket thuc. <<<\n";
                break;
            }
        }
        else if (!chessGame.Has_Legal_Moves(colorKing))
        {
            std::cout << "Hòa do không còn nước đi hợp lệ";
            break;
        }

        std::string moveStr;

        // Giả sử AI là phe Đen
        if (currentMode == GameMode::PvE && chessGame.sideToMove == 'b')
        {
            std::cout << "AI dang suy nghi...\n";
            // Lấy chuỗi FEN từ bàn cờ hiện tại
            std::string currentFEN = chessGame.GetFen();
            moveStr = ai.getBestMove(currentFEN);
            std::cout << "AI di nuoc: " << moveStr << std::endl;
        }
        else
        {
            std::cout << "Nhap nuoc di cua ban (vd: e2e4) \n";
            std::cout << "Neu muon di lai nhap Undo \n";
            std::cout << "Khong cho di lai Redo \n";
            std::cin >> moveStr;
        }
        if (to_lower(moveStr) == "undo")
        {
            if (chessGame.Undo())
            {
                chessGame.sideToMove = (chessGame.sideToMove == 'w') ? 'b' : 'w';
                std::cout << "Da quay lai luot truoc cua ban.\n";
            }
            else
                std::cout << "Chua the di lai\n";
            continue;
        }
        else if (to_lower(moveStr) == "redo")
        {
            // Chỉ Redo được nếu trước đó đã Undo
            if (chessGame.Redo())
            {
                chessGame.sideToMove = (chessGame.sideToMove == 'w') ? 'b' : 'w';
                std::cout << "Da thuc hien lai nuoc di vua xoa.\n";
            }
            else
                std::cout << "Nguoi choi chua di lai\n";
            continue;
        }
        if (Is_Valid_Input(moveStr))
        {

            std::pair<int, int> start = convert_to_XY(moveStr.substr(0, 2));
            std::pair<int, int> dest = convert_to_XY(moveStr.substr(2, 2));

            // Thực hiện di chuyển và cập nhật trạng thái
            chessGame.Execute_Move(start.first, start.second, dest.first, dest.second);
        }
        else
        {
            std::cout << "Nhan Enter de tiep tuc...";
            std::cin.ignore();
            std::cin.get();
        }
    }
}