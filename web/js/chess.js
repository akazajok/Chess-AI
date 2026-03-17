
const boardElement = document.getElementById('chess-board');
const rowLabelsLeft = document.getElementById('row-labels-left');
const rowLabelsRight = document.getElementById('row-labels-right');
const colLabelsTop = document.getElementById('col-labels-top');
const colLabelsBottom = document.getElementById('col-labels-bottom');

// Ánh xạ ký tự FEN sang đường dẫn file ảnh
const pieceImages = {
    'K': 'piece/wK.jpg', 'Q': 'piece/wQ.jpg', 'R': 'piece/wR.jpg', 'B': 'piece/wB.jpg', 'N': 'piece/wN.jpg', 'P': 'piece/wP.jpg',
    'k': 'piece/bK.jpg', 'q': 'piece/bQ.jpg', 'r': 'piece/bR.jpg', 'b': 'piece/bB.jpg', 'n': 'piece/bN.jpg', 'p': 'piece/bP.jpg'
};

let currentFEN = "";
let selectedSquare = null; // Lưu ID ô đang chọn (VD: 'e2')


//==============Section ID cần thiết implement==========//
const newGameBtn = document.getElementById('new-game-btn');
const moveListEl = document.getElementById('move-list');
const statusText = document.getElementById('game-status');
const capturedWhiteList = document.getElementById('captured-white-list');
const capturedBlackList = document.getElementById('captured-black-list');

// Hàm tạo bàn cờ ( nhãn hàng && cột, 64 ô cờ sáng tối)
function createBoard() {
    const cols = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'];

    // tạo hàng ngang a-h 
    for (let i = 0; i < 8; ++i) {
        const top = document.createElement('span');
        top.innerText = cols[i];
        colLabelsTop.appendChild(top);

        const bottom = document.createElement('span');
        bottom.innerText = cols[i];
        colLabelsBottom.appendChild(bottom);
    }

    // tạo 64 ô cờ và nhãn hàng 8-1
    for (let row = 8; row >= 1; row--) {
        const left = document.createElement('span');
        left.innerText = row;
        rowLabelsLeft.appendChild(left);

        const right = document.createElement('span');
        right.innerText = row;
        rowLabelsRight.appendChild(right);

        for (let col = 0; col < 8; ++col) {
            const square = document.createElement('div');
            square.classList.add('square');
            square.id = cols[col] + row;

            const isLight = (row + col) % 2 === 0;
            if (isLight)
                square.classList.add('light');
            else
                square.classList.add("dark");

            // Lắng nghe sự kiện click
            square.addEventListener('click', () => handleSquareClick(square.id));

            boardElement.appendChild(square);
        }
    }
}

// Đưa quân lên bàn cờ
function loadFen(fenString) {
    const boardPart = fenString.split(' ')[0];

    let row = 8;
    let col = 0;
    const cols = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'];

    for (let i = 0; i < boardPart.length; ++i) {
        const char = boardPart[i];
        // xuống hàng
        if (char === '/') {
            row--;
            col = 0;
        }
        else if (!isNaN(char)) {
            // Nếu là con số -> ô trống
            col += parseInt(char);
        }
        else {
            // 1. Lấy cái ô cờ cần gắn quân 
            const squareId = cols[col] + row;
            const squareElement = document.getElementById(squareId);

            // 2. Tạo thẻ img
            const pieceElement = document.createElement('img');

            // 3. Gắn img vào thẻ
            pieceElement.src = pieceImages[char];

            // 4. Làm đẹp nhờ css
            pieceElement.classList.add('piece');

            // 5. Nhét vào ô cờ 
            squareElement.appendChild(pieceElement);

            col++;
        }
    }
}
//===================New game function===============//
// Xóa toàn bộ quân trên bàn cờ
function clearBoardPieces() {
    const pieceImgs = document.querySelectorAll('.square img');
    pieceImgs.forEach(img => img.remove());
}

// Reset các panel phụ trợ
function resetSidePanels() {
    if (moveListEl) moveListEl.innerHTML = ''; //rêset history
    if (capturedWhiteList) capturedWhiteList.innerHTML = '';
    if (capturedBlackList) capturedBlackList.innerHTML = '';
    if (statusText) statusText.textContent = 'White to move'; //reset status
}

// Gọi API new game xuống server (respawn engine, nhận FEN)
async function requestNewGame() {
    const resp = await fetch('http://localhost:3000/api/newgame', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({})
    });
    const json = await resp.json();
    return json.result || defaultFEN;
}

// Xử lý khi bấm New Game
async function handleNewGame() {
    try {
        const fen = await requestNewGame();
        selectedSquare = null;
        currentFEN = fen;
        clearBoardPieces();
        loadFen(fen);
        resetSidePanels();
    } catch (err) {
        console.error('New Game lỗi, fallback FEN mặc định', err);
        selectedSquare = null;
        currentFEN = defaultFEN;
        clearBoardPieces();
        loadFen(defaultFEN);
        resetSidePanels();
    }
}

// Xóa toàn bộ quân cờ 
function clearFen() {
    const allSquares = document.querySelectorAll('.square');
    for (let i = 0; i < allSquares.length; ++i) {
        const square = allSquares[i];
        const piece = square.querySelector('img');
        if (piece)
            square.removeChild(piece);
    }
}

// Xóa gợi ý nước đi
function clearHighlights() {
    const allSquares = document.querySelectorAll('.square');
    for (let i = 0; i < allSquares.length; ++i) {
        allSquares[i].classList.remove('valid-move', 'valid-capture');
    }
}

function handleSquareClick(squareId) {
    // lấy ô đang được click - ô hiện tại
    const clickSquare = document.getElementById(squareId);

    // xem ô đó có quân cờ hay không 
    const pieceInSquare = clickSquare.querySelector('img');

    // bấm lần 1 - trước đó chưa chọn quân nào 
    if (selectedSquare === null) {
        if (pieceInSquare) {
            selectedSquare = squareId;
            // đổi màu quân cờ đang được click
            clickSquare.classList.add('selected');

            getValidMovesFromServer(squareId);
        }
    }
    else {
        // lấy ô cũ bấm ở lần 1
        const oldSquare = document.getElementById(selectedSquare);

        if (selectedSquare == squareId) {
            selectedSquare = null;
            oldSquare.classList.remove('selected');
            clearHighlights();
        }
        else // click vào ô khác
        {
            const moveString = oldSquare.id + clickSquare.id; // VD: "e2e4"
            sendMoveToServer(moveString);

            // DỌN DẸP SAU KHI ĐI:
            oldSquare.classList.remove('selected'); // Tắt hiệu ứng sáng ở ô cũ
            selectedSquare = null; // Trả hệ thống về Trạng thái 0 chờ nước đi tiếp theo

            clearHighlights();
        }
    }
}

// Hàm gửi yêu cầu lấy nước đi và hiển thị
async function getValidMovesFromServer(squareId) {
    try {
        // --- PHẦN 1: MÔ PHỎNG DỮ LIỆU ĐỂ TEST GIAO DIỆN ---
        // Tạm thời giả lập: cứ click vào đâu thì hiện dấu chấm ở 2 ô phía trước nó
        let colChar = squareId.charAt(0); // VD: 'e'
        let rowNum = parseInt(squareId.charAt(1)); // VD: 2

        const mockMoves = [
            { id: colChar + (rowNum + 1), isCapture: false }, // Bước tới 1 ô (hiện dấu chấm)
            { id: colChar + (rowNum + 2), isCapture: true }   // Bước tới 2 ô (hiện vòng tròn đỏ)
        ];

        // Vẽ lên giao diện
        showValidMoves(mockMoves);

        /* // --- PHẦN 2: CODE THẬT SAU NÀY (Bỏ comment khi đã code xong C++ và sever.js) ---
        const response = await fetch('http://localhost:3000/api/valid-moves', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ square: squareId }) 
        });
        const json = await response.json();
        showValidMoves(json.moves); 
        */

    } catch (error) {
        console.error("Lỗi khi lấy danh sách gợi ý:", error);
    }
}

// Hàm gán class CSS để hiện thị dấu chấm
function showValidMoves(moves) {
    for (let i = 0; i < moves.length; i++) {
        const move = moves[i];
        const targetSquare = document.getElementById(move.id);

        if (targetSquare) {
            // Nếu là nước ăn quân (isCapture = true), dùng class valid-capture
            if (move.isCapture) {
                targetSquare.classList.add('valid-capture');
            }
            // Nếu là ô trống, dùng class valid-move
            else {
                targetSquare.classList.add('valid-move');
            }
        }
    }
}

// Hàm gửi nước đi lên Node.js và nhận FEN mới từ C++
async function sendMoveToServer(moveStr) {
    try {
        // Gọi API của Node.js
        // await nghĩa là "đợi ở đây cho đến khi có phản hồi thì mới chạy tiếp dòng dưới".
        const response = await fetch('http://localhost:3000/api/move', {
            method: 'POST', // Phương thức POST giống với app.post bên sever.js
            headers: {
                'Content-Type': 'application/json' // Báo cho server biết mình gửi dạng JSON
            },
            body: JSON.stringify({ data: moveStr }) // Đóng gói dữ liệu: { "data": "e2e4" }
        });

        // Đợi Node.js trả kết quả (chính là chuỗi FEN từ C++)
        const json = await response.json();
        const result = json.result;

        // --- PHÂN TÍCH KẾT QUẢ TỪ C++ ---
        if (result.startsWith("CHECKMATE")) {
            const parts = result.split("|");
            const winner = (parts[1] === "White") ? "Quân trắng chiến thắng" : "Quân đen chiến thắng";
            showGameOver(winner, "Chiếu bí (Checkmate)");
            currentFEN = parts[2];
        }
        else if (result.startsWith("KING_CAPTURED")) {
            const parts = result.split("|");
            const winner = (parts[1] === "White") ? "Quân Trắng Thắng!" : "Quân Đen Thắng!";
            showGameOver(winner, "Vua đã bị tiêu diệt!");
            currentFEN = parts[2];
        }
        else if (result.startsWith("DRAW")) {
            const parts = result.split("|");
            showGameOver("Hòa cờ", parts[1]);
            currentFEN = parts[2];
        }
        else if (result != "INVALID") {
            currentFEN = result;
        }
        else {
            console.warn("Nước đi không hợp lệ!");
            return;
        }

        clearFen();
        loadFen(currentFEN);

    } catch (error) {
        console.error("Lỗi khi gọi server:", error);
        alert("Lỗi kết nối Server! Vui lòng bật Node.js");
    }
}

function showGameOver(winner, reason) {
    // 1. Dòng này sẽ xóa chữ "Trắng thắng!" đi và thay bằng chữ bạn truyền vào
    document.getElementById('winner-text').innerText = winner;

    // 2. Dòng này xóa chữ "Chiếu bí" đi và thay bằng lý do bạn truyền vào
    document.getElementById('reason-text').innerText = reason;

    // 3. Bật bảng thông báo lên
    document.getElementById('game-over-modal').classList.add('active');
}

// Chạy ngay khi web vừa load/F5
sendMoveToServer("reset");

// Tạo bàn cờ 
createBoard();

// Đẩy quân lên bàn cờ
loadFen(currentFEN);

// Bind nút New Game
if (newGameBtn)
    newGameBtn.addEventListener('click', handleNewGame);
