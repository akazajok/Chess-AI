const boardElement = document.getElementById('chess-board');
const rowLabelsLeft = document.getElementById('row-labels-left');
const rowLabelsRight = document.getElementById('row-labels-right');
const colLabelsTop = document.getElementById('col-labels-top');
const colLabelsBottom = document.getElementById('col-labels-bottom');

// ===== UI controls =====
const newGameBtn = document.getElementById('new-game-btn');
const undoBtn = document.getElementById('undo-btn');
const redoBtn = document.getElementById('redo-btn');
const moveListEl = document.getElementById('move-list');
const statusText = document.getElementById('game-status');
const capturedWhiteList = document.getElementById('captured-white-list');
const capturedBlackList = document.getElementById('captured-black-list');
const topTimerEl = document.getElementById('top-timer');
const bottomTimerEl = document.getElementById('bottom-timer');
const timerSelectEl = document.getElementById('timer-select');
const topPlayerEl = document.getElementById('top-player');
const bottomPlayerEl = document.getElementById('bottom-player');

// ===== Asset paths (dễ đổi về sau) =====
const PIECE_ASSET_BASE = 'piece/';
const pieceImages = {
    K: `${PIECE_ASSET_BASE}wK.png`, Q: `${PIECE_ASSET_BASE}wQ.png`, R: `${PIECE_ASSET_BASE}wR.png`,
    B: `${PIECE_ASSET_BASE}wB.png`, N: `${PIECE_ASSET_BASE}wN.png`, P: `${PIECE_ASSET_BASE}wP.png`,
    k: `${PIECE_ASSET_BASE}bK.png`, q: `${PIECE_ASSET_BASE}bQ.png`, r: `${PIECE_ASSET_BASE}bR.png`,
    b: `${PIECE_ASSET_BASE}bB.png`, n: `${PIECE_ASSET_BASE}bN.png`, p: `${PIECE_ASSET_BASE}bP.png`
};

const defaultFEN = 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1';

let currentFEN = defaultFEN;
let selectedSquare = null;
let gameEnded = false;

// Captured pieces theo lịch sử nước đi (không suy từ FEN để tránh sai khi promotion)
let capturedByWhite = []; // trắng ăn quân đen
let capturedByBlack = []; // đen ăn quân trắng
let appliedMoves = [];    // stack đã đi
let redoMoves = [];       // stack đã undo

// ===== Timer state =====
let timerId = null;
let whiteTime = 300;
let blackTime = 300;

// ===== Helpers =====
function getSideToMove(fen) {
    const parts = fen.split(' ');
    return parts[1] || 'w';
}

function formatTime(seconds) {
    const s = Math.max(0, seconds);
    const mm = Math.floor(s / 60);
    const ss = s % 60;
    return `${mm}:${ss.toString().padStart(2, '0')}`;
}

function extractBoardPart(fen) {
    return fen.split(' ')[0] || '';
}

function fenToBoardMap(fen) {
    const board = new Map();
    const boardPart = extractBoardPart(fen);
    const rows = boardPart.split('/');

    for (let r = 0; r < 8; r++) {
        let c = 0;
        for (const ch of rows[r]) {
            if (!Number.isNaN(Number(ch))) {
                c += Number(ch);
                continue;
            }
            const square = String.fromCharCode('a'.charCodeAt(0) + c) + (8 - r);
            board.set(square, ch);
            c++;
        }
    }
    return board;
}

function inferCapturedPiece(beforeFen, moveStr) {
    const from = moveStr.slice(0, 2);
    const to = moveStr.slice(2, 4);
    const board = fenToBoardMap(beforeFen);
    const movingPiece = board.get(from);
    if (!movingPiece) return null;

    // Capture thường
    const directCapture = board.get(to);
    if (directCapture) return directCapture;

    // En passant: tốt đi chéo vào ô trống
    const isPawn = movingPiece.toLowerCase() === 'p';
    const isDiagonalMove = Math.abs(from.charCodeAt(0) - to.charCodeAt(0)) === 1 &&
        Math.abs(Number(from[1]) - Number(to[1])) === 1;
    if (isPawn && isDiagonalMove) {
        const epSquare = `${to[0]}${from[1]}`;
        return board.get(epSquare) || null;
    }

    return null;
}

function renderCapturedPieces() {
    capturedBlackList.innerHTML = '';
    capturedWhiteList.innerHTML = '';

    for (const ch of capturedByWhite) {
        const img = document.createElement('img');
        img.src = pieceImages[ch];
        img.className = 'piece';
        img.style.width = '22px';
        img.style.height = '22px';
        img.style.marginRight = '4px';
        capturedBlackList.appendChild(img);
    }

    for (const ch of capturedByBlack) {
        const img = document.createElement('img');
        img.src = pieceImages[ch];
        img.className = 'piece';
        img.style.width = '22px';
        img.style.height = '22px';
        img.style.marginRight = '4px';
        capturedWhiteList.appendChild(img);
    }
}

function updateStatusByFen(fen) {
    if (gameEnded) return;
    const side = getSideToMove(fen);
    statusText.textContent = side === 'w' ? 'White to move' : 'Black to move';

    if (side === 'w') {
        bottomPlayerEl.classList.add('active');
        topPlayerEl.classList.remove('active');
    } else {
        topPlayerEl.classList.add('active');
        bottomPlayerEl.classList.remove('active');
    }
}

function updateTimerUI() {
    bottomTimerEl.textContent = formatTime(whiteTime);
    topTimerEl.textContent = formatTime(blackTime);
}

function stopClock() {
    if (timerId) {
        clearInterval(timerId);
        timerId = null;
    }
}

function startClock() {
    stopClock();
    if (gameEnded) return;

    // No timer mode
    if (Number(timerSelectEl.value) === 0) {
        bottomTimerEl.textContent = '∞';
        topTimerEl.textContent = '∞';
        return;
    }

    timerId = setInterval(() => {
        const side = getSideToMove(currentFEN);

        if (side === 'w') whiteTime--;
        else blackTime--;

        updateTimerUI();

        if (whiteTime <= 0) {
            gameEnded = true;
            stopClock();
            showGameOver('Black wins!', 'White hết thời gian');
        }
        if (blackTime <= 0) {
            gameEnded = true;
            stopClock();
            showGameOver('White wins!', 'Black hết thời gian');
        }
    }, 1000);
}

function resetClockBySelection() {
    const selected = Number(timerSelectEl.value);
    whiteTime = selected;
    blackTime = selected;
    updateTimerUI();
}

// ===== Board Rendering =====
function createBoard() {
    const cols = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'];

    for (let i = 0; i < 8; i++) {
        const top = document.createElement('span');
        top.innerText = cols[i];
        colLabelsTop.appendChild(top);

        const bottom = document.createElement('span');
        bottom.innerText = cols[i];
        colLabelsBottom.appendChild(bottom);
    }

    for (let row = 8; row >= 1; row--) {
        const left = document.createElement('span');
        left.innerText = row;
        rowLabelsLeft.appendChild(left);

        const right = document.createElement('span');
        right.innerText = row;
        rowLabelsRight.appendChild(right);

        for (let col = 0; col < 8; col++) {
            const square = document.createElement('div');
            square.classList.add('square');
            square.id = cols[col] + row;
            square.classList.add((row + col) % 2 === 0 ? 'light' : 'dark');
            square.addEventListener('click', () => handleSquareClick(square.id));
            boardElement.appendChild(square);
        }
    }
}

function clearBoardPieces() {
    document.querySelectorAll('.square img').forEach(img => img.remove());
}

function clearHighlights() {
    document.querySelectorAll('.square').forEach(sq => {
        sq.classList.remove('valid-move', 'valid-capture', 'selected');
    });
}

function loadFen(fenString) {
    const boardPart = extractBoardPart(fenString);
    let row = 8;
    let col = 0;
    const cols = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'];

    for (const char of boardPart) {
        if (char === '/') {
            row--;
            col = 0;
            continue;
        }
        if (!Number.isNaN(Number(char))) {
            col += Number(char);
            continue;
        }

        const squareId = cols[col] + row;
        const squareElement = document.getElementById(squareId);
        if (squareElement && pieceImages[char]) {
            const pieceElement = document.createElement('img');
            pieceElement.src = pieceImages[char];
            pieceElement.classList.add('piece');
            squareElement.appendChild(pieceElement);
        }
        col++;
    }
}

function refreshBoardFromFen(fen) {
    currentFEN = fen;
    clearBoardPieces();
    clearHighlights();
    selectedSquare = null;
    loadFen(fen);
    updateStatusByFen(fen);
    renderCapturedPieces();
}

// ===== API calls =====
async function postJson(url, body = {}) {
    const response = await fetch(url, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(body)
    });
    return response.json();
}

function parseResult(result) {
    if (result.startsWith('CHECKMATE')) {
        const parts = result.split('|');
        showGameOver(parts[1] === 'White' ? 'White wins!' : 'Black wins!', 'Chiếu bí (Checkmate)');
        gameEnded = true;
        stopClock();
        return parts[2];
    }

    if (result.startsWith('KING_CAPTURED')) {
        const parts = result.split('|');
        showGameOver(parts[1] === 'White' ? 'White wins!' : 'Black wins!', 'Vua đã bị bắt');
        gameEnded = true;
        stopClock();
        return parts[2];
    }

    if (result.startsWith('DRAW')) {
        const parts = result.split('|');
        showGameOver('Hòa cờ', parts[1]);
        gameEnded = true;
        stopClock();
        return parts[2];
    }

    if (result === 'INVALID') return null;
    return result;
}

async function sendMoveToServer(moveStr) {
    const json = await postJson('http://localhost:3000/api/move', { data: moveStr });
    const fen = parseResult(json.result);
    if (!fen) return false;
    refreshBoardFromFen(fen);
    return true;
}

async function requestNewGame() {
    const json = await postJson('http://localhost:3000/api/newgame', {});
    return json.result || defaultFEN;
}

async function requestUndo() {
    const json = await postJson('http://localhost:3000/api/undo', {});
    return json.result;
}

async function requestRedo() {
    const json = await postJson('http://localhost:3000/api/redo', {});
    return json.result;
}

// ===== Game actions =====
async function handleNewGame() {
    try {
        const fen = await requestNewGame();
        gameEnded = false;
        document.getElementById('game-over-modal').classList.remove('active');
        if (moveListEl) moveListEl.innerHTML = '';
        capturedByWhite = [];
        capturedByBlack = [];
        appliedMoves = [];
        redoMoves = [];
        resetClockBySelection();
        refreshBoardFromFen(fen);
        startClock();
    } catch (error) {
        console.error('New Game lỗi:', error);
        alert('Không gọi được New Game API');
    }
}

async function handleUndo() {
    if (gameEnded) return;
    try {
        const result = await requestUndo();
        const fen = parseResult(result);
        if (!fen) return;

        if (appliedMoves.length > 0) {
            const last = appliedMoves.pop();
            redoMoves.push(last);
            if (last.capturedPiece) {
                if (last.mover === 'w') capturedByWhite.pop();
                else capturedByBlack.pop();
            }
        }

        refreshBoardFromFen(fen);
    } catch (error) {
        console.error('Undo lỗi:', error);
    }
}

async function handleRedo() {
    if (gameEnded) return;
    try {
        const result = await requestRedo();
        const fen = parseResult(result);
        if (!fen) return;

        if (redoMoves.length > 0) {
            const again = redoMoves.pop();
            appliedMoves.push(again);
            if (again.capturedPiece) {
                if (again.mover === 'w') capturedByWhite.push(again.capturedPiece);
                else capturedByBlack.push(again.capturedPiece);
            }
        }

        refreshBoardFromFen(fen);
    } catch (error) {
        console.error('Redo lỗi:', error);
    }
}

function addMoveToHistory(moveStr) {
    if (!moveListEl) return;
    const item = document.createElement('div');
    item.textContent = moveStr;
    moveListEl.appendChild(item);
    moveListEl.scrollTop = moveListEl.scrollHeight;
}

function handleSquareClick(squareId) {
    if (gameEnded) return;

    const clickedSquare = document.getElementById(squareId);
    const pieceInSquare = clickedSquare.querySelector('img');

    if (selectedSquare === null) {
        if (!pieceInSquare) return;
        selectedSquare = squareId;
        clickedSquare.classList.add('selected');
        return;
    }

    const oldSquare = document.getElementById(selectedSquare);

    if (selectedSquare === squareId) {
        selectedSquare = null;
        oldSquare.classList.remove('selected');
        clearHighlights();
        return;
    }

    const moveString = oldSquare.id + clickedSquare.id;
    const beforeFen = currentFEN;
    const mover = getSideToMove(beforeFen);
    const capturedPiece = inferCapturedPiece(beforeFen, moveString);
    oldSquare.classList.remove('selected');
    selectedSquare = null;

    sendMoveToServer(moveString)
        .then(success => {
            if (success) {
                appliedMoves.push({ move: moveString, mover, capturedPiece });
                redoMoves = [];
                if (capturedPiece) {
                    if (mover === 'w') capturedByWhite.push(capturedPiece);
                    else capturedByBlack.push(capturedPiece);
                }
                addMoveToHistory(moveString);
                renderCapturedPieces();
            }
        })
        .catch(error => {
            console.error('Lỗi khi gọi server:', error);
            alert('Lỗi kết nối server!');
        });
}

function showGameOver(winner, reason) {
    document.getElementById('winner-text').innerText = winner;
    document.getElementById('reason-text').innerText = reason;
    document.getElementById('game-over-modal').classList.add('active');
}

// ===== Bootstrap =====
function bindEvents() {
    if (newGameBtn) newGameBtn.addEventListener('click', handleNewGame);
    if (undoBtn) undoBtn.addEventListener('click', handleUndo);
    if (redoBtn) redoBtn.addEventListener('click', handleRedo);
    if (timerSelectEl) {
        timerSelectEl.addEventListener('change', () => {
            resetClockBySelection();
            startClock();
        });
    }
}

async function init() {
    createBoard();
    bindEvents();
    await handleNewGame();
}

init();
