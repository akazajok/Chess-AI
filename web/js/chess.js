
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
        let char = boardPart[i];
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
            let squareId = cols[col] + row;
            let squareElement = document.getElementById(squareId);

            // 2. Tạo thẻ img
            let pieceElement = document.createElement('img');

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

// Tạo bàn cờ 
createBoard();

// Đẩy quân lên bàn cờ
loadFen(currentFEN);
