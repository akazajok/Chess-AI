// ============================================================
//  CHESS AI — Full Web UI with Timer + Stockfish + Flip
// ============================================================

// ====== CONSTANTS ======
const PIECE_UNICODE = {
    'K': '♔', 'Q': '♕', 'R': '♖', 'B': '♗', 'N': '♘', 'P': '♙',
    'k': '♚', 'q': '♛', 'r': '♜', 'b': '♝', 'n': '♞', 'p': '♟'
};
const PIECE_VALUE = { 'p': 1, 'n': 3, 'b': 3, 'r': 5, 'q': 9, 'k': 0 };
const PIECE_LABEL = { 'Q': 'Queen', 'R': 'Rook', 'B': 'Bishop', 'N': 'Knight' };
const COLS = 'abcdefgh';
const STARTING_FEN = 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1';

// ====== GAME STATE ======
let board = [];
let currentPlayer = 'white';
let selectedSquare = null;
let lastMove = null;
let moveHistory = [];
let redoStack = [];
let capturedWhite = [];
let capturedBlack = [];
let flipped = false;
let gameOver = false;
let moveCount = 0;
let castling = { K: true, Q: true, k: true, q: true };
let enPassantTarget = null;
let halfmoveClock = 0;
let pendingPromotion = null;

// ====== TIMER STATE ======
let timerEnabled = true;
let whiteTime = 300;  // seconds
let blackTime = 300;
let timerInterval = null;
let timerStarted = false;

// ====== AI STATE ======
let gameMode = 'pvp';   // 'pvp' or 'pve'
let aiColor = 'black';
let aiDepth = 10;
let aiThinking = false;
let stockfish = null;

// ============================================================
//  FEN
// ============================================================
function parseFEN(fen) {
    const parts = fen.split(' ');
    const rows = parts[0].split('/');
    const b = [];
    for (let r = 0; r < 8; r++) {
        b[r] = [];
        let c = 0;
        for (const ch of rows[r]) {
            if (ch >= '1' && ch <= '8') { for (let i = 0; i < parseInt(ch); i++) b[r][c++] = null; }
            else b[r][c++] = ch;
        }
    }
    currentPlayer = (parts[1] === 'b') ? 'black' : 'white';
    castling = { K: false, Q: false, k: false, q: false };
    if (parts[2] !== '-') for (const ch of parts[2]) castling[ch] = true;
    if (parts[3] !== '-') {
        enPassantTarget = [8 - parseInt(parts[3][1]), parts[3].charCodeAt(0) - 97];
    } else enPassantTarget = null;
    halfmoveClock = parseInt(parts[4]) || 0;
    moveCount = parseInt(parts[5]) || 1;
    return b;
}

function toFEN() {
    let fen = '';
    for (let r = 0; r < 8; r++) {
        let empty = 0;
        for (let c = 0; c < 8; c++) {
            if (board[r][c]) { if (empty > 0) { fen += empty; empty = 0; } fen += board[r][c]; }
            else empty++;
        }
        if (empty > 0) fen += empty;
        if (r < 7) fen += '/';
    }
    fen += ' ' + (currentPlayer === 'white' ? 'w' : 'b');
    let cr = '';
    if (castling.K) cr += 'K'; if (castling.Q) cr += 'Q';
    if (castling.k) cr += 'k'; if (castling.q) cr += 'q';
    fen += ' ' + (cr || '-');
    fen += enPassantTarget ? ' ' + COLS[enPassantTarget[1]] + (8 - enPassantTarget[0]) : ' -';
    fen += ' ' + halfmoveClock + ' ' + moveCount;
    return fen;
}

// ============================================================
//  HELPERS
// ============================================================
function isWhite(p) { return p && p === p.toUpperCase(); }
function pieceColor(p) { return p ? (isWhite(p) ? 'white' : 'black') : null; }
function sameColor(a, b) { return pieceColor(a) === pieceColor(b); }
function inBounds(r, c) { return r >= 0 && r < 8 && c >= 0 && c < 8; }
function cloneBoard(b) { return b.map(row => [...row]); }
function toAlgebraic(r, c) { return COLS[c] + (8 - r); }
function capitalize(s) { return s.charAt(0).toUpperCase() + s.slice(1); }

function findKing(color, b) {
    const k = color === 'white' ? 'K' : 'k';
    for (let r = 0; r < 8; r++) for (let c = 0; c < 8; c++) if (b[r][c] === k) return [r, c];
    return null;
}

// ============================================================
//  MOVE GENERATION
// ============================================================
function getRawMoves(fromR, fromC, b) {
    const piece = b[fromR][fromC];
    if (!piece) return [];
    const moves = [];
    const type = piece.toUpperCase();
    const color = pieceColor(piece);
    const dir = color === 'white' ? -1 : 1;

    const addIf = (r, c) => {
        if (!inBounds(r, c)) return false;
        if (b[r][c] && sameColor(piece, b[r][c])) return false;
        moves.push([r, c]);
        return !b[r][c];
    };
    const slide = (dr, dc) => {
        let r = fromR + dr, c = fromC + dc;
        while (inBounds(r, c)) { if (!addIf(r, c)) break; r += dr; c += dc; }
    };

    switch (type) {
        case 'P': {
            const startRow = color === 'white' ? 6 : 1;
            if (inBounds(fromR + dir, fromC) && !b[fromR + dir][fromC]) {
                moves.push([fromR + dir, fromC]);
                if (fromR === startRow && !b[fromR + 2 * dir][fromC]) moves.push([fromR + 2 * dir, fromC]);
            }
            for (const dc of [-1, 1]) {
                const nr = fromR + dir, nc = fromC + dc;
                if (!inBounds(nr, nc)) continue;
                if (b[nr][nc] && !sameColor(piece, b[nr][nc])) moves.push([nr, nc]);
                if (enPassantTarget && enPassantTarget[0] === nr && enPassantTarget[1] === nc) moves.push([nr, nc]);
            }
            break;
        }
        case 'N': for (const [dr, dc] of [[-2,-1],[-2,1],[-1,-2],[-1,2],[1,-2],[1,2],[2,-1],[2,1]]) addIf(fromR+dr, fromC+dc); break;
        case 'B': slide(-1,-1); slide(-1,1); slide(1,-1); slide(1,1); break;
        case 'R': slide(-1,0); slide(1,0); slide(0,-1); slide(0,1); break;
        case 'Q': slide(-1,-1); slide(-1,1); slide(1,-1); slide(1,1); slide(-1,0); slide(1,0); slide(0,-1); slide(0,1); break;
        case 'K':
            for (const [dr, dc] of [[-1,-1],[-1,0],[-1,1],[0,-1],[0,1],[1,-1],[1,0],[1,1]]) addIf(fromR+dr, fromC+dc);
            // Castling
            if (color === 'white' && fromR === 7 && fromC === 4) {
                if (castling.K && !b[7][5] && !b[7][6] && b[7][7] === 'R'
                    && !isSquareAttacked(7,4,'black',b) && !isSquareAttacked(7,5,'black',b) && !isSquareAttacked(7,6,'black',b))
                    moves.push([7, 6]);
                if (castling.Q && !b[7][3] && !b[7][2] && !b[7][1] && b[7][0] === 'R'
                    && !isSquareAttacked(7,4,'black',b) && !isSquareAttacked(7,3,'black',b) && !isSquareAttacked(7,2,'black',b))
                    moves.push([7, 2]);
            }
            if (color === 'black' && fromR === 0 && fromC === 4) {
                if (castling.k && !b[0][5] && !b[0][6] && b[0][7] === 'r'
                    && !isSquareAttacked(0,4,'white',b) && !isSquareAttacked(0,5,'white',b) && !isSquareAttacked(0,6,'white',b))
                    moves.push([0, 6]);
                if (castling.q && !b[0][3] && !b[0][2] && !b[0][1] && b[0][0] === 'r'
                    && !isSquareAttacked(0,4,'white',b) && !isSquareAttacked(0,3,'white',b) && !isSquareAttacked(0,2,'white',b))
                    moves.push([0, 2]);
            }
            break;
    }
    return moves;
}

// ============================================================
//  ATTACK DETECTION
// ============================================================
function isSquareAttacked(row, col, byColor, b) {
    for (let r = 0; r < 8; r++) for (let c = 0; c < 8; c++) {
        const p = b[r][c];
        if (!p || pieceColor(p) !== byColor) continue;
        const type = p.toUpperCase();
        const dr = row - r, dc = col - c, absR = Math.abs(dr), absC = Math.abs(dc);
        if (type === 'P') { if (dr === (byColor === 'white' ? -1 : 1) && absC === 1) return true; }
        else if (type === 'N') { if ((absR===2&&absC===1)||(absR===1&&absC===2)) return true; }
        else if (type === 'B') { if (absR===absC&&absR>0&&pathClear(r,c,row,col,b)) return true; }
        else if (type === 'R') { if ((dr===0||dc===0)&&(absR+absC>0)&&pathClear(r,c,row,col,b)) return true; }
        else if (type === 'Q') { if ((absR===absC||dr===0||dc===0)&&(absR+absC>0)&&pathClear(r,c,row,col,b)) return true; }
        else if (type === 'K') { if (absR<=1&&absC<=1) return true; }
    }
    return false;
}

function pathClear(fR, fC, tR, tC, b) {
    const sr = Math.sign(tR-fR), sc = Math.sign(tC-fC);
    let r = fR+sr, c = fC+sc;
    while (r !== tR || c !== tC) { if (b[r][c]) return false; r += sr; c += sc; }
    return true;
}

function isInCheck(color, b) {
    const king = findKing(color, b);
    if (!king) return false;
    return isSquareAttacked(king[0], king[1], color === 'white' ? 'black' : 'white', b);
}

// ============================================================
//  LEGAL MOVES
// ============================================================
function getLegalMoves(fromR, fromC) {
    const piece = board[fromR][fromC];
    if (!piece) return [];
    const color = pieceColor(piece);
    return getRawMoves(fromR, fromC, board).filter(([toR, toC]) => {
        const sim = cloneBoard(board);
        sim[toR][toC] = sim[fromR][fromC]; sim[fromR][fromC] = null;
        if (piece.toUpperCase() === 'P' && enPassantTarget && toR === enPassantTarget[0] && toC === enPassantTarget[1]) {
            sim[color === 'white' ? toR + 1 : toR - 1][toC] = null;
        }
        return !isInCheck(color, sim);
    });
}

function hasLegalMoves(color) {
    for (let r = 0; r < 8; r++) for (let c = 0; c < 8; c++)
        if (board[r][c] && pieceColor(board[r][c]) === color && getLegalMoves(r, c).length > 0) return true;
    return false;
}

// ============================================================
//  EXECUTE MOVE
// ============================================================
function executeMove(fromR, fromC, toR, toC, promotionPiece) {
    const piece = board[fromR][fromC];
    const captured = board[toR][toC];
    const color = pieceColor(piece);
    const type = piece.toUpperCase();

    const record = {
        from: [fromR, fromC], to: [toR, toC], piece, captured,
        castling: { ...castling }, enPassant: enPassantTarget ? [...enPassantTarget] : null,
        halfmoveClock, special: null, promotedTo: null
    };

    // En passant
    if (type === 'P' && enPassantTarget && toR === enPassantTarget[0] && toC === enPassantTarget[1]) {
        const capR = color === 'white' ? toR + 1 : toR - 1;
        record.captured = board[capR][toC]; record.special = 'enpassant'; board[capR][toC] = null;
    }
    // Castling
    if (type === 'K' && Math.abs(toC - fromC) === 2) {
        record.special = 'castling';
        if (toC === 6) { board[fromR][5] = board[fromR][7]; board[fromR][7] = null; }
        if (toC === 2) { board[fromR][3] = board[fromR][0]; board[fromR][0] = null; }
    }

    board[toR][toC] = piece; board[fromR][fromC] = null;

    // Promotion
    if (type === 'P' && (toR === 0 || toR === 7)) {
        const promo = promotionPiece || (color === 'white' ? 'Q' : 'q');
        board[toR][toC] = promo; record.special = 'promotion'; record.promotedTo = promo;
    }

    // En passant target
    enPassantTarget = (type === 'P' && Math.abs(toR - fromR) === 2) ? [(fromR + toR) / 2, fromC] : null;

    // Castling rights
    if (type === 'K') { if (color === 'white') { castling.K = castling.Q = false; } else { castling.k = castling.q = false; } }
    if (type === 'R') {
        if (fromR===7&&fromC===0) castling.Q=false; if (fromR===7&&fromC===7) castling.K=false;
        if (fromR===0&&fromC===0) castling.q=false; if (fromR===0&&fromC===7) castling.k=false;
    }
    if (toR===7&&toC===0) castling.Q=false; if (toR===7&&toC===7) castling.K=false;
    if (toR===0&&toC===0) castling.q=false; if (toR===0&&toC===7) castling.k=false;

    if (type === 'P' || captured) halfmoveClock = 0; else halfmoveClock++;
    if (color === 'black') moveCount++;

    if (record.captured) {
        (pieceColor(record.captured) === 'white' ? capturedWhite : capturedBlack).push(record.captured);
    }

    record.fen = toFEN();
    moveHistory.push(record);
    redoStack = [];
    lastMove = { from: [fromR, fromC], to: [toR, toC] };
    currentPlayer = currentPlayer === 'white' ? 'black' : 'white';

    // Start timer on first move
    if (!timerStarted && timerEnabled) { timerStarted = true; startTimer(); }
}

// ============================================================
//  MOVE NOTATION
// ============================================================
function getMoveNotation(record) {
    const type = record.piece.toUpperCase();
    const dest = toAlgebraic(record.to[0], record.to[1]);
    if (record.special === 'castling') return record.to[1] === 6 ? 'O-O' : 'O-O-O';
    let n = '';
    if (type !== 'P') n += type;
    if (record.captured) n += (type === 'P' ? COLS[record.from[1]] : '') + 'x';
    n += dest;
    if (record.special === 'promotion') n += '=' + record.promotedTo.toUpperCase();
    return n;
}

// ============================================================
//  UNDO / REDO
// ============================================================
function undoMove() {
    if (moveHistory.length === 0 || aiThinking) return;
    const rec = moveHistory.pop();
    redoStack.push(rec);
    const [fR, fC] = rec.from, [tR, tC] = rec.to;

    board[fR][fC] = rec.special === 'promotion' ? rec.piece : board[tR][tC];
    board[tR][tC] = null;

    if (rec.special === 'enpassant') {
        board[pieceColor(rec.piece) === 'white' ? tR + 1 : tR - 1][tC] = rec.captured;
    } else if (rec.captured) board[tR][tC] = rec.captured;

    if (rec.special === 'castling') {
        if (tC === 6) { board[fR][7] = board[fR][5]; board[fR][5] = null; }
        if (tC === 2) { board[fR][0] = board[fR][3]; board[fR][3] = null; }
    }

    castling = { ...rec.castling }; enPassantTarget = rec.enPassant; halfmoveClock = rec.halfmoveClock;
    if (rec.captured) (pieceColor(rec.captured) === 'white' ? capturedWhite : capturedBlack).pop();
    if (pieceColor(rec.piece) === 'black') moveCount--;

    currentPlayer = currentPlayer === 'white' ? 'black' : 'white';
    lastMove = moveHistory.length > 0 ? { from: moveHistory.at(-1).from, to: moveHistory.at(-1).to } : null;
    selectedSquare = null; gameOver = false;
}

function redoMove() {
    if (redoStack.length === 0 || aiThinking) return;
    const rec = redoStack.pop();
    const savedRedo = [...redoStack];
    executeMove(rec.from[0], rec.from[1], rec.to[0], rec.to[1], rec.promotedTo);
    redoStack = savedRedo;
}

// ============================================================
//  GAME STATE
// ============================================================
function checkGameState() {
    const status = document.getElementById('game-status');
    const inCheck = isInCheck(currentPlayer, board);
    const hasMove = hasLegalMoves(currentPlayer);

    if (!hasMove) {
        gameOver = true; stopTimer();
        if (inCheck) {
            const winner = currentPlayer === 'white' ? 'Black' : 'White';
            status.textContent = `Checkmate! ${winner} wins!`;
            status.className = 'status-text checkmate';
        } else { status.textContent = 'Stalemate! Draw.'; status.className = 'status-text draw'; }
    } else if (inCheck) {
        status.textContent = `${capitalize(currentPlayer)} is in Check!`;
        status.className = 'status-text check';
    } else if (halfmoveClock >= 100) {
        status.textContent = '50-move rule. Draw.'; status.className = 'status-text draw';
        gameOver = true; stopTimer();
    } else {
        status.textContent = `${capitalize(currentPlayer)} to move`;
        status.className = 'status-text';
    }
}

// ============================================================
//  TIMER
// ============================================================
function formatTime(secs) {
    if (secs <= 0) return '0:00';
    const m = Math.floor(secs / 60);
    const s = secs % 60;
    return m + ':' + (s < 10 ? '0' : '') + s;
}

function startTimer() {
    stopTimer();
    if (!timerEnabled) return;
    timerInterval = setInterval(() => {
        if (gameOver || pendingPromotion || aiThinking) return;
        if (currentPlayer === 'white') {
            whiteTime--;
            if (whiteTime <= 0) { whiteTime = 0; gameOver = true; stopTimer();
                document.getElementById('game-status').textContent = 'Time out! Black wins!';
                document.getElementById('game-status').className = 'status-text checkmate';
            }
        } else {
            blackTime--;
            if (blackTime <= 0) { blackTime = 0; gameOver = true; stopTimer();
                document.getElementById('game-status').textContent = 'Time out! White wins!';
                document.getElementById('game-status').className = 'status-text checkmate';
            }
        }
        renderTimers();
    }, 1000);
}

function stopTimer() { if (timerInterval) { clearInterval(timerInterval); timerInterval = null; } }

function renderTimers() {
    // Top player timer and bottom player timer depend on flip
    const topColor = flipped ? 'white' : 'black';
    const botColor = flipped ? 'black' : 'white';
    const topTime = topColor === 'white' ? whiteTime : blackTime;
    const botTime = botColor === 'white' ? whiteTime : blackTime;

    const topEl = document.getElementById('top-timer');
    const botEl = document.getElementById('bottom-timer');
    topEl.textContent = formatTime(topTime);
    botEl.textContent = formatTime(botTime);

    topEl.className = 'timer' + (topTime <= 30 && topTime > 0 ? ' low' : '') + (topTime <= 0 ? ' timeout' : '');
    botEl.className = 'timer' + (botTime <= 30 && botTime > 0 ? ' low' : '') + (botTime <= 0 ? ' timeout' : '');
}

// ============================================================
//  PROMOTION UI
// ============================================================
function showPromotionModal(fromR, fromC, toR, toC) {
    pendingPromotion = { fromR, fromC, toR, toC };
    const color = pieceColor(board[fromR][fromC]);
    const pieces = color === 'white' ? ['Q','R','B','N'] : ['q','r','b','n'];
    const container = document.getElementById('promotion-choices');
    container.innerHTML = '';

    pieces.forEach(p => {
        const btn = document.createElement('div');
        btn.className = 'promotion-choice';

        const icon = document.createElement('span');
        icon.textContent = PIECE_UNICODE[p];
        btn.appendChild(icon);

        const label = document.createElement('span');
        label.className = 'promo-label';
        label.textContent = PIECE_LABEL[p.toUpperCase()];
        btn.appendChild(label);

        btn.addEventListener('click', () => {
            executeMove(fromR, fromC, toR, toC, p);
            document.getElementById('promotion-modal').classList.remove('active');
            pendingPromotion = null; selectedSquare = null;
            checkGameState(); renderAll();
            tryAIMove();
        });
        container.appendChild(btn);
    });

    document.getElementById('promotion-modal').classList.add('active');
}

// ============================================================
//  STOCKFISH AI
// ============================================================
const STOCKFISH_CDN = 'https://cdnjs.cloudflare.com/ajax/libs/stockfish.js/10.0.2/stockfish.js';

function initStockfish() {
    if (stockfish) return; // already initialized

    document.getElementById('ai-status').textContent = '⏳ Loading Stockfish...';

    // Fetch the script text and create a Blob Worker (avoids CORS issues with new Worker(cross-origin-url))
    fetch(STOCKFISH_CDN)
        .then(res => {
            if (!res.ok) throw new Error('HTTP ' + res.status);
            return res.text();
        })
        .then(code => {
            const blob = new Blob([code], { type: 'application/javascript' });
            const url = URL.createObjectURL(blob);
            stockfish = new Worker(url);
            setupStockfishHandlers();
        })
        .catch(e => {
            console.warn('Stockfish Blob Worker failed, trying direct Worker:', e);
            // Fallback: direct Worker URL (works if served from same-origin HTTP server)
            try {
                stockfish = new Worker(STOCKFISH_CDN);
                setupStockfishHandlers();
            } catch (e2) {
                console.warn('Stockfish direct Worker also failed:', e2);
                stockfish = null;
                document.getElementById('ai-status').textContent = '⚠ Serve via HTTP server for AI';
            }
        });
}

function setupStockfishHandlers() {
    stockfish.onmessage = function(event) {
        const msg = typeof event.data === 'string' ? event.data : '';
        handleStockfishMessage(msg);
    };
    stockfish.onerror = function(err) {
        console.warn('Stockfish worker error:', err);
        document.getElementById('ai-status').textContent = '⚠ Engine error';
    };
    sendToStockfish('uci');
    sendToStockfish('isready');
    document.getElementById('ai-status').textContent = '✅ Stockfish ready';
    console.log('Stockfish initialized');
    tryAIMove();
}

function sendToStockfish(cmd) {
    if (!stockfish) return;
    if (typeof stockfish.postMessage === 'function') stockfish.postMessage(cmd);
}

function handleStockfishMessage(msg) {
    if (typeof msg !== 'string') return;

    if (msg.startsWith('bestmove')) {
        const parts = msg.split(' ');
        const bestMove = parts[1];
        if (bestMove && bestMove !== '(none)' && aiThinking) {
            applyAIMove(bestMove);
        }
        aiThinking = false;
        document.getElementById('ai-status').textContent = '';
    }
}

function tryAIMove() {
    if (gameMode !== 'pve' || gameOver || aiThinking || pendingPromotion) return;
    if (currentPlayer !== aiColor) return;
    if (!stockfish) {
        document.getElementById('ai-status').textContent = '⚠ Stockfish not loaded';
        return;
    }

    aiThinking = true;
    document.getElementById('ai-status').textContent = '🤔 Thinking...';

    const fen = toFEN();
    sendToStockfish('position fen ' + fen);
    sendToStockfish('go depth ' + aiDepth);
}

function applyAIMove(moveStr) {
    // Parse UCI move like "e2e4" or "e7e8q" (promotion)
    const fromC = moveStr.charCodeAt(0) - 97;
    const fromR = 8 - parseInt(moveStr[1]);
    const toC = moveStr.charCodeAt(2) - 97;
    const toR = 8 - parseInt(moveStr[3]);
    let promo = null;
    if (moveStr.length === 5) {
        const promoChar = moveStr[4];
        promo = aiColor === 'white' ? promoChar.toUpperCase() : promoChar.toLowerCase();
    }

    if (!inBounds(fromR, fromC) || !inBounds(toR, toC)) { aiThinking = false; return; }

    executeMove(fromR, fromC, toR, toC, promo);
    selectedSquare = null;
    checkGameState();
    renderAll();
}

// ============================================================
//  CLICK HANDLER
// ============================================================
function handleClick(row, col) {
    if (gameOver || pendingPromotion || aiThinking) return;
    if (gameMode === 'pve' && currentPlayer === aiColor) return;

    const clickedPiece = board[row][col];

    if (selectedSquare === null) {
        if (clickedPiece && pieceColor(clickedPiece) === currentPlayer) selectedSquare = { row, col };
    } else {
        const { row: fR, col: fC } = selectedSquare;
        if (fR === row && fC === col) { selectedSquare = null; }
        else if (clickedPiece && pieceColor(clickedPiece) === currentPlayer) { selectedSquare = { row, col }; }
        else {
            const legal = getLegalMoves(fR, fC);
            if (legal.some(([r, c]) => r === row && c === col)) {
                const piece = board[fR][fC];
                if (piece.toUpperCase() === 'P' && (row === 0 || row === 7)) {
                    showPromotionModal(fR, fC, row, col);
                    return;
                }
                executeMove(fR, fC, row, col);
                selectedSquare = null;
                checkGameState();
                renderAll();
                tryAIMove();
                return;
            } else { selectedSquare = null; }
        }
    }
    renderAll();
}

// ============================================================
//  RENDER
// ============================================================
function renderBoard() {
    const boardEl = document.getElementById('chess-board');
    boardEl.innerHTML = '';
    const inCheck = isInCheck(currentPlayer, board);
    const kingPos = findKing(currentPlayer, board);
    const legalMoves = selectedSquare ? getLegalMoves(selectedSquare.row, selectedSquare.col) : [];

    for (let vR = 0; vR < 8; vR++) {
        for (let vC = 0; vC < 8; vC++) {
            const row = flipped ? 7 - vR : vR;
            const col = flipped ? 7 - vC : vC;
            const square = document.createElement('div');
            square.className = 'square ' + ((row + col) % 2 === 0 ? 'light' : 'dark');

            if (selectedSquare && selectedSquare.row === row && selectedSquare.col === col) square.classList.add('selected');
            if (lastMove && ((row===lastMove.from[0]&&col===lastMove.from[1])||(row===lastMove.to[0]&&col===lastMove.to[1]))) square.classList.add('last-move');
            if (inCheck && kingPos && row === kingPos[0] && col === kingPos[1]) square.classList.add('in-check');
            if (selectedSquare && legalMoves.some(([r,c]) => r===row&&c===col)) square.classList.add(board[row][col] ? 'valid-capture' : 'valid-move');

            const piece = board[row][col];
            if (piece) {
                const span = document.createElement('span');
                span.className = 'piece'; span.textContent = PIECE_UNICODE[piece];
                square.appendChild(span);
            }
            square.addEventListener('click', () => handleClick(row, col));
            boardEl.appendChild(square);
        }
    }
}

function renderMoveList() {
    const list = document.getElementById('move-list');
    list.innerHTML = '';
    for (let i = 0; i < moveHistory.length; i += 2) {
        const row = document.createElement('div'); row.className = 'move-row';
        const num = document.createElement('span'); num.className = 'move-number'; num.textContent = (Math.floor(i/2)+1) + '.';
        row.appendChild(num);
        const wm = document.createElement('span'); wm.className = 'move-white'; wm.textContent = getMoveNotation(moveHistory[i]);
        row.appendChild(wm);
        if (i+1 < moveHistory.length) {
            const bm = document.createElement('span'); bm.className = 'move-black'; bm.textContent = getMoveNotation(moveHistory[i+1]);
            row.appendChild(bm);
        }
        list.appendChild(row);
    }
    list.scrollTop = list.scrollHeight;
}

function renderCaptured() {
    const sort = arr => arr.slice().sort((a,b) => PIECE_VALUE[b.toLowerCase()] - PIECE_VALUE[a.toLowerCase()]);
    document.getElementById('captured-white-list').innerHTML = sort(capturedWhite).map(p => PIECE_UNICODE[p]).join(' ');
    document.getElementById('captured-black-list').innerHTML = sort(capturedBlack).map(p => PIECE_UNICODE[p]).join(' ');

    // Score diff
    const wScore = capturedBlack.reduce((s, p) => s + PIECE_VALUE[p.toLowerCase()], 0);
    const bScore = capturedWhite.reduce((s, p) => s + PIECE_VALUE[p.toLowerCase()], 0);
    const diff = wScore - bScore;
    document.getElementById('score-white').textContent = diff > 0 ? '+' + diff : '';
    document.getElementById('score-black').textContent = diff < 0 ? '+' + Math.abs(diff) : '';
}

function renderPlayerInfo() {
    const topEl = document.getElementById('top-player');
    const botEl = document.getElementById('bottom-player');
    const topDot = document.getElementById('top-dot');
    const botDot = document.getElementById('bottom-dot');
    const topName = document.getElementById('top-name');
    const botName = document.getElementById('bottom-name');

    const topColor = flipped ? 'white' : 'black';
    const botColor = flipped ? 'black' : 'white';

    topName.textContent = capitalize(topColor) + (gameMode === 'pve' && aiColor === topColor ? ' (AI)' : '');
    botName.textContent = capitalize(botColor) + (gameMode === 'pve' && aiColor === botColor ? ' (AI)' : '');

    topDot.className = 'player-dot ' + topColor + '-dot';
    botDot.className = 'player-dot ' + botColor + '-dot';

    topEl.classList.toggle('active', currentPlayer === topColor);
    botEl.classList.toggle('active', currentPlayer === botColor);
}

function renderLabels() {
    const rowL = document.getElementById('row-labels'); rowL.innerHTML = '';
    const colL = document.getElementById('col-labels'); colL.innerHTML = '';
    for (let i = 0; i < 8; i++) {
        const rs = document.createElement('span'); rs.textContent = flipped ? (i+1) : (8-i); rowL.appendChild(rs);
        const cs = document.createElement('span'); cs.textContent = flipped ? COLS[7-i] : COLS[i]; colL.appendChild(cs);
    }
}

function updateButtons() {
    document.getElementById('undo-btn').disabled = moveHistory.length === 0 || aiThinking;
    document.getElementById('redo-btn').disabled = redoStack.length === 0 || aiThinking;
}

function renderAll() {
    renderBoard(); renderMoveList(); renderCaptured();
    renderPlayerInfo(); renderLabels(); renderTimers(); updateButtons();
}

// ============================================================
//  NEW GAME
// ============================================================
function newGame() {
    stopTimer();
    board = parseFEN(STARTING_FEN);
    selectedSquare = null; lastMove = null;
    moveHistory = []; redoStack = [];
    capturedWhite = []; capturedBlack = [];
    gameOver = false; pendingPromotion = null; aiThinking = false;

    const timerSecs = parseInt(document.getElementById('timer-select').value);
    timerEnabled = timerSecs > 0;
    whiteTime = timerSecs; blackTime = timerSecs;
    timerStarted = false;

    checkGameState(); renderAll();

    // If AI plays white, trigger AI move
    if (gameMode === 'pve' && aiColor === 'white') {
        setTimeout(() => tryAIMove(), 300);
    }
}

// ============================================================
//  EVENT LISTENERS
// ============================================================
document.getElementById('undo-btn').addEventListener('click', () => {
    if (gameMode === 'pve' && moveHistory.length >= 2) {
        // Undo both AI move and human move
        undoMove(); undoMove();
    } else {
        undoMove();
    }
    checkGameState(); renderAll();
});
document.getElementById('redo-btn').addEventListener('click', () => {
    if (gameMode === 'pve' && redoStack.length >= 2) {
        redoMove(); redoMove();
    } else {
        redoMove();
    }
    checkGameState(); renderAll();
});
document.getElementById('new-game-btn').addEventListener('click', newGame);

document.getElementById('flip-btn').addEventListener('click', () => { flipped = !flipped; renderAll(); });

// Mode buttons
document.getElementById('pvp-btn').addEventListener('click', () => {
    gameMode = 'pvp';
    document.getElementById('pvp-btn').classList.add('active');
    document.getElementById('pve-btn').classList.remove('active');
    document.getElementById('ai-settings').style.display = 'none';
    newGame();
});

document.getElementById('pve-btn').addEventListener('click', () => {
    gameMode = 'pve';
    document.getElementById('pve-btn').classList.add('active');
    document.getElementById('pvp-btn').classList.remove('active');
    document.getElementById('ai-settings').style.display = 'block';
    if (!stockfish) initStockfish();
    newGame();
});

// AI depth
document.getElementById('ai-depth').addEventListener('input', (e) => {
    aiDepth = parseInt(e.target.value);
    document.getElementById('depth-label').textContent = aiDepth;
});

// AI color
document.getElementById('play-white-btn').addEventListener('click', () => {
    aiColor = 'black';
    document.getElementById('play-white-btn').classList.add('active');
    document.getElementById('play-black-btn').classList.remove('active');
    newGame();
});

document.getElementById('play-black-btn').addEventListener('click', () => {
    aiColor = 'white';
    flipped = true;
    document.getElementById('play-black-btn').classList.add('active');
    document.getElementById('play-white-btn').classList.remove('active');
    newGame();
});

// Timer select
document.getElementById('timer-select').addEventListener('change', () => newGame());

// Keyboard shortcuts
document.addEventListener('keydown', (e) => {
    if (e.ctrlKey && e.key === 'z') { e.preventDefault(); undoMove(); checkGameState(); renderAll(); }
    if (e.ctrlKey && e.key === 'y') { e.preventDefault(); redoMove(); checkGameState(); renderAll(); }
});

// ============================================================
//  INIT
// ============================================================
newGame();