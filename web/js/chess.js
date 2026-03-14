
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

let selectedSquare = null; // Lưu ID ô đang chọn (VD: 'e2')
let currentFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

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
        }
    }
    else {
        // lấy ô cũ bấm ở lần 1
        const oldSquare = document.getElementById(selectedSquare);

        if (selectedSquare == squareId) {
            selectedSquare = null;
            oldSquare.classList.remove('selected');
        }
        else // click vào ô khác
        {
            // lấy ảnh quân cũ
            const pieceToMove = oldSquare.querySelector('img');

            // // Nếu đó là ô địch thì xóa ảnh quân cờ đi
            // if (pieceInSquare) {
            //     clickSquare.removeChild(pieceInSquare);
            // }

            // // Lệnh appendChild sẽ tự động "rút" thẻ img từ ô cũ và "cắm" nó sang ô mới.
            // // không cần viết code xóa thẻ img ở ô cũ, JS tự động di dời nó!
            // clickSquare.appendChild(pieceToMove);

            // // DỌN DẸP SAU KHI ĐI:
            // oldSquare.classList.remove('selected'); // Tắt hiệu ứng sáng ở ô cũ
            // selectedSquare = null; // Trả hệ thống về Trạng thái 0 chờ nước đi tiếp theo

            const moveString = oldSquare.id + clickSquare.id; // VD: "e2e4"
            sendMoveToServer(moveString);
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
        const newFen = json.result;

        // --- Cập nhật lại giao diện ---
        currentFEN = newFen; // Cập nhật biến lưu trữ
        clearBoard();        // Quét sạch bàn cờ cũ
        loadFen(currentFEN); // Xếp lại cờ theo FEN mới nhất của C++

    } catch (error) {
        console.error("Lỗi khi gọi server:", error);
    }
}

// Tạo bàn cờ 
createBoard();

// Đẩy quân lên bàn cờ
loadFen(currentFEN);

