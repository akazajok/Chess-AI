#include "../include/gamePlay.h"
#include "../include/Stockfish.h"

gameManager::gameManager() : rowKing(-1), colKing(-1), colorKing(Color::None) {}

void gameManager::Init_Game(std::string FEN, GameMode mode, const int &aiLevel)
{
    chessGame.Set_Up_Board(FEN); // Khởi tạo quân cờ lên grid
    currentMode = mode;
    currentLevel = aiLevel;
}

bool gameManager::Is_Valid_Input(const std::string &moveStr)
{

    // 1. Kiểm tra định dạng chuỗi nhập vào (vd: e2e4)
    if (!isValidMoveFormat(moveStr))
        return false;

    std::pair<int, int> start = convert_to_XY(moveStr.substr(0, 2));
    std::pair<int, int> dest = convert_to_XY(moveStr.substr(2, 2));

    Piece *p = chessGame.Get_Piece_At(start.first, start.second);

    // 2. Kiểm tra ô bắt đầu có quân và có đúng lượt hay không
    if (!p)
        return false;

    char currentTurn = chessGame.sideToMove;
    if ((currentTurn == 'w' && p->Get_Color() != Color::White) ||
        (currentTurn == 'b' && p->Get_Color() != Color::Black))
    {

        return false;
    }
    // 3. Kiểm tra luật di chuyển của quân cờ đó
    if (!chessGame.Can_Move(start.first, start.second, dest.first, dest.second))
    {

        return false;
    }

    // 4. Kiểm tra xem sau nước đi Vua có bị chiếu tướng không
    int tempRowKing = (p->Get_Name() == Name::King) ? dest.first : rowKing;
    int tempColKing = (p->Get_Name() == Name::King) ? dest.second : colKing;

    if (!chessGame.Is_Safe_Move(p, dest.first, dest.second, tempRowKing, tempColKing, colorKing))
    {
        return false;
    }

    return true;
}

void gameManager::Game_Turn()
{

    // Khởi tạo AI (đảm bảo đường dẫn tới file .exe chính xác)
    Stockfish ai("stockfish-windows-x86-64-avx2.exe");

    // Set độ khó
    ai.setSkillLevel(currentLevel);

    // Lấy nước đi tối ưu
    auto getAIMoveWithLoading = [&ai](const std::string &currentFEN)
    {
        // Tạo một luồng bất đồng bộ (chạy ngầm) để gọi AI
        // std::async sẽ đẩy việc gọi hàm getBestMove sang một luồng (thread) khác
        std::future<std::string> futureMove = std::async(std::launch::async, [&ai, currentFEN]()
                                                         { return ai.getBestMove(currentFEN); });

        // Vòng lặp này giúp giao diện không bị đơ.
        // Nó kiểm tra xem AI đã nghĩ xong chưa sau mỗi 100 mili-giây
        while (futureMove.wait_for(std::chrono::milliseconds(100)) != std::future_status::ready)
        {
            // Trong lúc rảnh rỗi chờ AI, mình in ra các dấu chấm tạo hiệu ứng "loading..."
            std::cout << "." << std::flush;
        }
        std::cout << "\n"; // Xuống dòng sau khi in xong hiệu ứng chấm chấm
        // Lấy kết quả từ luồng chạy ngầm sau khi nó hoàn thành
        return futureMove.get();
    };

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
        }
        colorKing = king->Get_Color();

        chessGame.Display(); // Hiển thị bàn cờ ra console
        std::cout << chessGame.enPassantTarget << " " << chessGame.fullmoveNumber << '\n';
        std::cout << "=> Danh gia the tran (Evaluation): " << ai.lastEvaluation << std::endl;

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
            std::cout << "Hoa do luat 50 nuoc";
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
            std::cout << "Hoa do khong con nuoc di hop le";
            break;
        }

        std::string moveStr;

        // Giả sử AI là phe Đen
        if ((currentMode == GameMode::PvE && chessGame.sideToMove == 'b'))
        {
            std::cout << "AI dang suy nghi";

            moveStr = getAIMoveWithLoading(chessGame.GetFen());

            std::cout << "AI di nuoc: " << moveStr << std::endl;
            std::cout << "=> Danh gia the tran (Evaluation): " << ai.lastEvaluation << std::endl;
        }
        else
        {
            std::cout << "Nhap nuoc di cua ban (vd: e2e4) \n";
            std::cout << "Neu muon di lai nhap Undo \n";
            std::cout << "Khong cho di lai Redo \n";
            std::cout << "Goi y nuoc di Hint \n";
            std::cin >> moveStr;
        }

        if (to_lower(moveStr) == "undo")
        {
            if (chessGame.Undo())
            {
                if (currentMode == GameMode::PvE)
                    chessGame.Undo();
                else
                    chessGame.sideToMove = (chessGame.sideToMove == 'w') ? 'b' : 'w';
                std::cout << "Da quay lai luot truoc cua ban.\n";
            }
            else
                std::cout << "Chua the di lai\n";
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
        }
        else if (to_lower(moveStr) == "hint")
        {
            std::cout << "AI dang phan tich the tran de goi y";

            std::string bestMove = getAIMoveWithLoading(chessGame.GetFen());

            std::cout << "=> Nuoc di AI goi y: " << bestMove << "\n";
            std::cout << "=> Danh gia the tran (Evaluation): " << ai.lastEvaluation << "\n\n";

            continue;
        }
        else if (Is_Valid_Input(moveStr))
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

std::string gameManager::Check_Game_State()
{
    // Lấy thông tin Vua của phe ĐANG ĐẾN LƯỢT (Ví dụ: Đen vừa đi xong, giờ là lượt Trắng)
    char currentTurn = chessGame.sideToMove;
    int rK = (currentTurn == 'w') ? chessGame.rowKingWhite : chessGame.rowKingBlack;
    int cK = (currentTurn == 'w') ? chessGame.colKingWhite : chessGame.colKingBlack;
    Color cColor = (currentTurn == 'w') ? Color::White : Color::Black;

    // Đối thủ của phe hiện tại (Dùng để in ra người thắng nếu phe hiện tại thua)
    std::string opponent = (currentTurn == 'w') ? "Black" : "White";

    // 1. Kiểm tra xem Vua có bị ăn mất không (Trường hợp cờ biến thể/lỗi game)
    Piece *king = chessGame.Get_Piece_At(rK, cK);
    if (!king || king->Get_Name() != Name::King)
    {
        return "KING_CAPTURED|" + opponent + "|" + chessGame.GetFen();
    }

    // 2. Kiểm tra Hòa cờ do thiếu quân hoặc luật 50 nước
    if (chessGame.Is_Insufficient_Material())
    {
        return "DRAW|Thiếu quân chiếu bí (Insufficient Material)|" + chessGame.GetFen();
    }

    if (chessGame.Is_Draw_By_50_Moves())
    {
        return "DRAW|Luật 50 nước (50-move rule)|" + chessGame.GetFen();
    }

    // 3. Kiểm tra Chiếu / Chiếu Bí / Hòa Pat (Stalemate)
    if (chessGame.Get_Checking_Piece(rK, cK, cColor))
    {
        // Bị chiếu mà hết đường lui -> Chiếu bí
        if (!chessGame.Can_Escape_Check(rK, cK, cColor))
        {
            return "CHECKMATE|" + opponent + "|" + chessGame.GetFen();
        }
    }
    else if (!chessGame.Has_Legal_Moves(cColor))
    {
        // Không bị chiếu nhưng không thể đi -> Hòa Pat
        return "DRAW|Hết nước đi (Stalemate)|" + chessGame.GetFen();
    }

    // 4. Nếu game vẫn bình thường -> Chỉ trả về chuỗi FEN
    return chessGame.GetFen();
}

std::string gameManager::Process_Web_Move(const std::string &moveStr)
{
    if (moveStr.size() == 2)
    {
        std::pair<int, int> pos = convert_to_XY(moveStr);
        Piece *piece = chessGame.Get_Piece_At(pos.first, pos.second);

        if (piece)
        {
            std::vector<MoveInfor> moves = chessGame.getValidMoves(pos.first, pos.second);

            // Xây dựng chuỗi JSON
            std::string jsonResult = "[";
            for (size_t i = 0; i < moves.size(); ++i)
            {
                jsonResult += "{\"squareId\":\"" + moves[i].squareId + "\",\"isCapture\":" + (moves[i].isCapture ? "true" : "false") + "}";
                if (i < moves.size() - 1)
                    jsonResult += ",";
            }
            jsonResult += "]";

            // LẬP TỨC TRẢ VỀ CHUỖI JSON (Ngăn code chạy tiếp xuống dưới)
            return jsonResult;
        }
        else
        {
            return "[]"; // Trả về mảng rỗng nếu click vào ô không có quân
        }
    }

    if (moveStr == "reset")
        return chessGame.GetFen();

    // 1: Cập nhật tọa độ Vua cho class để hàm Is_Valid_Input không bị lỗi -1,-1
    rowKing = (chessGame.sideToMove == 'w') ? chessGame.rowKingWhite : chessGame.rowKingBlack;
    colKing = (chessGame.sideToMove == 'w') ? chessGame.colKingWhite : chessGame.colKingBlack;
    colorKing = (chessGame.sideToMove == 'w') ? Color::White : Color::Black;

    // 2: Kiểm tra nước đi có đúng luật không
    if (!Is_Valid_Input(moveStr))
    {
        return "INVALID";
    }

    // 3: Thực hiện nước đi
    std::pair<int, int> start = convert_to_XY(moveStr.substr(0, 2));
    std::pair<int, int> dest = convert_to_XY(moveStr.substr(2, 2));

    // Hàm này chạy xong sẽ TỰ ĐỘNG đổi chessGame.sideToMove sang phe đối thủ
    chessGame.Execute_Move(start.first, start.second, dest.first, dest.second);

    // 4: Gọi hàm kiểm tra cục diện cho phe vừa được nhường lượt
    return Check_Game_State();
}