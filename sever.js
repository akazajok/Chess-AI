// khai báo thư viện
const express = require('express'); // tạo ra máy chủ web
const cors = require('cors'); // giấy phép để lên web
const { spawn } = require('child_process'); // chạy ngầm chessGame.exe

const app = express();
app.use(cors());
app.use(express.json()); // để sever đọc được dữ liệu từ web

const PORT = 3000;      // Cổng mạng số 3000

// Khởi chạy file C++ (Lưu ý đường dẫn tới file exe của bạn)
// Nếu file exe nằm trong thư mục gốc thì để './ChessGame.exe'
// Nếu nó nằm trong thư mục build hay src thì phải sửa lại, ví dụ: './src/ChessGame.exe'
let chessEngine = null;

function startEngine() {
    if (chessEngine) {
        chessEngine.kill();
        chessEngine = null;
        currentResponse = null;
    }

    chessEngine = spawn('ChessGame.exe');

    chessEngine.stdout.on('data', (data) => {
        const output = data.toString().trim();
        console.log(`[C++] trả về: ${output}`);

        if (currentResponse) {
            currentResponse.json({ result: output });
            currentResponse = null;
        }
    });

    chessEngine.stderr.on('data', (data) => {
        console.error(`[C++] Lỗi: ${data}`);
    });
}

// khởi động lần đầu
startEngine();

let currentResponse = null; // lưu thông tin đối tượng nào của WEB vừa hỏi C++

// stdout (Standard Output) === std::cout trong C++.
// Bất cứ khi nào mã C++ của bạn chạy lệnh cout << "kết quả";, Node.js sẽ ngay lập tức "nghe" thấy ở đoạn code này.
// Sau khi bắt được chữ C++ in ra (output), nó kiểm tra cuốn sổ xem có ai đang đợi không (if currentResponse). Nếu có, nó đóng gói kết quả lại và gửi trả về cho Web (.json({ result: output })), rồi xé nháp (currentResponse = null) để chờ lượt tiếp theo.
// app.post: Mở ra một đường dẫn api/move. Bất cứ khi nào Web có nước đi mới, nó sẽ gọi vào đây.
// req (Request - Yêu cầu Web gửi lên) và res (Response - Kết quả Server sẽ trả về).
// Ta lấy chuỗi FEN từ req.body.data, sau đó cất cái res vào cuốn sổ currentResponse.
// stdin.write(data + '\n'):Đưa thông tin vào Terminal của C++, sau đó (\n chính là phím Enter). 
app.post('/api/move', (req, res) => {
    console.log("====== ĐÃ NHẬN ĐƯỢC REQUEST TỪ WEB! Nước đi:", req.body.data);
    const { data } = req.body;
    console.log(`[Web] gửi xuống C++: ${data}`);

    currentResponse = res;
    chessEngine.stdin.write(data + '\n');
});

// Reset ván mới: respawn engine và trả về FEN mặc định
app.post('/api/newgame', (req, res) => {
    console.log(`[Web] yêu cầu New Game`);
    startEngine();
    // Engine khi khởi động sẽ chờ nước đi đầu tiên, FEN mặc định trong main.cpp
    const defaultFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    res.json({ result: defaultFEN });
});

app.listen(PORT, () => {
    console.log(`✅ Server cầu nối đã chạy tại http://localhost:${PORT}`);
    console.log(`⏳ Đang chờ Web gọi xuống...`);
});