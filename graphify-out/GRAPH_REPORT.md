# Graph Report - D:\CODING\Chess-AI  (2026-04-21)

## Corpus Check
- 24 files · ~23,203 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 122 nodes · 271 edges · 24 communities detected
- Extraction: 72% EXTRACTED · 28% INFERRED · 0% AMBIGUOUS · INFERRED: 76 edges (avg confidence: 0.8)
- Token cost: 0 input · 0 output

## Community Hubs (Navigation)
- [[_COMMUNITY_Community 0|Community 0]]
- [[_COMMUNITY_Community 1|Community 1]]
- [[_COMMUNITY_Community 2|Community 2]]
- [[_COMMUNITY_Community 3|Community 3]]
- [[_COMMUNITY_Community 4|Community 4]]
- [[_COMMUNITY_Community 5|Community 5]]
- [[_COMMUNITY_Community 6|Community 6]]
- [[_COMMUNITY_Community 7|Community 7]]
- [[_COMMUNITY_Community 8|Community 8]]
- [[_COMMUNITY_Community 9|Community 9]]
- [[_COMMUNITY_Community 10|Community 10]]
- [[_COMMUNITY_Community 11|Community 11]]
- [[_COMMUNITY_Community 12|Community 12]]
- [[_COMMUNITY_Community 13|Community 13]]
- [[_COMMUNITY_Community 14|Community 14]]
- [[_COMMUNITY_Community 15|Community 15]]
- [[_COMMUNITY_Community 16|Community 16]]
- [[_COMMUNITY_Community 17|Community 17]]
- [[_COMMUNITY_Community 18|Community 18]]
- [[_COMMUNITY_Community 19|Community 19]]
- [[_COMMUNITY_Community 20|Community 20]]
- [[_COMMUNITY_Community 21|Community 21]]
- [[_COMMUNITY_Community 22|Community 22]]
- [[_COMMUNITY_Community 23|Community 23]]

## God Nodes (most connected - your core abstractions)
1. `Get_Name()` - 20 edges
2. `Get_Color()` - 19 edges
3. `Game_Turn()` - 16 edges
4. `Update_Position()` - 15 edges
5. `Execute_Move()` - 13 edges
6. `Undo()` - 10 edges
7. `Redo()` - 10 edges
8. `Process_Web_Move()` - 10 edges
9. `refreshBoardFromFen()` - 10 edges
10. `ExecuteSpecialMove()` - 9 edges

## Surprising Connections (you probably didn't know these)
- `Update_Position()` --calls--> `Set_Position()`  [INFERRED]
  D:\CODING\Chess-AI\src\Board.cpp → D:\CODING\Chess-AI\src\Piece.cpp
- `Is_Valid_Input()` --calls--> `isValidMoveFormat()`  [INFERRED]
  D:\CODING\Chess-AI\src\gamePlay.cpp → D:\CODING\Chess-AI\src\Utils.cpp
- `Set_Up_Board()` --calls--> `Get_Name()`  [INFERRED]
  D:\CODING\Chess-AI\src\Board.cpp → D:\CODING\Chess-AI\src\Piece.cpp
- `Set_Up_Board()` --calls--> `Get_Color()`  [INFERRED]
  D:\CODING\Chess-AI\src\Board.cpp → D:\CODING\Chess-AI\src\Piece.cpp
- `GetFen()` --calls--> `Process_Web_Move()`  [INFERRED]
  D:\CODING\Chess-AI\src\Board.cpp → D:\CODING\Chess-AI\src\gamePlay.cpp

## Communities

### Community 0 - "Community 0"
Cohesion: 0.23
Nodes (24): CanCastle(), ClearRedo(), Execute_Move(), ExecuteCastling(), ExecuteEnPassant(), ExecutePromotion(), ExecutePromotionWithPiece(), ExecuteSpecialMove() (+16 more)

### Community 1 - "Community 1"
Cohesion: 0.19
Nodes (20): Can_Escape_Check(), Can_Move(), Display(), Get_Checking_Piece(), Get_Piece_On_Path(), GetFen(), GetPieceChar(), Has_Legal_Moves() (+12 more)

### Community 2 - "Community 2"
Cohesion: 0.22
Nodes (9): ParseCastlingRights(), Set_Up_Board(), Init_Game(), Process_Web_Move(), main(), convert_to_XY(), isValidMoveFormat(), to_lower() (+1 more)

### Community 3 - "Community 3"
Cohesion: 0.36
Nodes (8): bindEvents(), createBoard(), formatTime(), handleNewGame(), init(), resetClockBySelection(), startClock(), updateTimerUI()

### Community 4 - "Community 4"
Cohesion: 0.32
Nodes (8): clearBoardPieces(), clearHighlights(), getSideToMove(), handleSquareClick(), refreshBoardFromFen(), renderCapturedPieces(), sendMoveToServer(), updateStatusByFen()

### Community 5 - "Community 5"
Cohesion: 0.4
Nodes (5): handleUndo(), parseResult(), requestUndo(), showGameOver(), stopClock()

### Community 6 - "Community 6"
Cohesion: 0.5
Nodes (4): handleRedo(), postJson(), requestNewGame(), requestRedo()

### Community 7 - "Community 7"
Cohesion: 0.5
Nodes (4): extractBoardPart(), fenToBoardMap(), inferCapturedPiece(), loadFen()

### Community 8 - "Community 8"
Cohesion: 1.0
Nodes (0): 

### Community 9 - "Community 9"
Cohesion: 1.0
Nodes (0): 

### Community 10 - "Community 10"
Cohesion: 1.0
Nodes (0): 

### Community 11 - "Community 11"
Cohesion: 1.0
Nodes (0): 

### Community 12 - "Community 12"
Cohesion: 1.0
Nodes (0): 

### Community 13 - "Community 13"
Cohesion: 1.0
Nodes (0): 

### Community 14 - "Community 14"
Cohesion: 1.0
Nodes (0): 

### Community 15 - "Community 15"
Cohesion: 1.0
Nodes (0): 

### Community 16 - "Community 16"
Cohesion: 1.0
Nodes (0): 

### Community 17 - "Community 17"
Cohesion: 1.0
Nodes (0): 

### Community 18 - "Community 18"
Cohesion: 1.0
Nodes (0): 

### Community 19 - "Community 19"
Cohesion: 1.0
Nodes (0): 

### Community 20 - "Community 20"
Cohesion: 1.0
Nodes (0): 

### Community 21 - "Community 21"
Cohesion: 1.0
Nodes (0): 

### Community 22 - "Community 22"
Cohesion: 1.0
Nodes (0): 

### Community 23 - "Community 23"
Cohesion: 1.0
Nodes (0): 

## Knowledge Gaps
- **Thin community `Community 8`** (2 nodes): `sever.js`, `startEngine()`
  Too small to be a meaningful cluster - may be noise or needs more connections extracted.
- **Thin community `Community 9`** (2 nodes): `Bishop()`, `Bishop.h`
  Too small to be a meaningful cluster - may be noise or needs more connections extracted.
- **Thin community `Community 10`** (2 nodes): `Board()`, `Board.h`
  Too small to be a meaningful cluster - may be noise or needs more connections extracted.
- **Thin community `Community 11`** (2 nodes): `King.h`, `Piece()`
  Too small to be a meaningful cluster - may be noise or needs more connections extracted.
- **Thin community `Community 12`** (2 nodes): `Knight.h`, `Knight()`
  Too small to be a meaningful cluster - may be noise or needs more connections extracted.
- **Thin community `Community 13`** (2 nodes): `Pawn.h`, `Piece()`
  Too small to be a meaningful cluster - may be noise or needs more connections extracted.
- **Thin community `Community 14`** (2 nodes): `Queen.h`, `Piece()`
  Too small to be a meaningful cluster - may be noise or needs more connections extracted.
- **Thin community `Community 15`** (2 nodes): `Rook.h`, `Piece()`
  Too small to be a meaningful cluster - may be noise or needs more connections extracted.
- **Thin community `Community 16`** (2 nodes): `Stockfish.h`, `Stockfish()`
  Too small to be a meaningful cluster - may be noise or needs more connections extracted.
- **Thin community `Community 17`** (2 nodes): `Is_Valid_Move()`, `Bishop.cpp`
  Too small to be a meaningful cluster - may be noise or needs more connections extracted.
- **Thin community `Community 18`** (2 nodes): `King.cpp`, `Is_Valid_Move()`
  Too small to be a meaningful cluster - may be noise or needs more connections extracted.
- **Thin community `Community 19`** (2 nodes): `Knight.cpp`, `Is_Valid_Move()`
  Too small to be a meaningful cluster - may be noise or needs more connections extracted.
- **Thin community `Community 20`** (2 nodes): `Pawn.cpp`, `Is_Valid_Move()`
  Too small to be a meaningful cluster - may be noise or needs more connections extracted.
- **Thin community `Community 21`** (2 nodes): `Queen.cpp`, `Is_Valid_Move()`
  Too small to be a meaningful cluster - may be noise or needs more connections extracted.
- **Thin community `Community 22`** (1 nodes): `Piece.h`
  Too small to be a meaningful cluster - may be noise or needs more connections extracted.
- **Thin community `Community 23`** (1 nodes): `Utils.h`
  Too small to be a meaningful cluster - may be noise or needs more connections extracted.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Game_Turn()` connect `Community 1` to `Community 0`, `Community 2`?**
  _High betweenness centrality (0.027) - this node is a cross-community bridge._
- **Why does `Process_Web_Move()` connect `Community 2` to `Community 0`, `Community 1`?**
  _High betweenness centrality (0.024) - this node is a cross-community bridge._
- **Why does `Get_Name()` connect `Community 1` to `Community 0`, `Community 2`?**
  _High betweenness centrality (0.024) - this node is a cross-community bridge._
- **Are the 19 inferred relationships involving `Get_Name()` (e.g. with `Set_Up_Board()` and `GetPieceChar()`) actually correct?**
  _`Get_Name()` has 19 INFERRED edges - model-reasoned connections that need verification._
- **Are the 18 inferred relationships involving `Get_Color()` (e.g. with `Set_Up_Board()` and `GetPieceChar()`) actually correct?**
  _`Get_Color()` has 18 INFERRED edges - model-reasoned connections that need verification._
- **Are the 14 inferred relationships involving `Game_Turn()` (e.g. with `Get_Name()` and `Get_Color()`) actually correct?**
  _`Game_Turn()` has 14 INFERRED edges - model-reasoned connections that need verification._
- **Are the 3 inferred relationships involving `Update_Position()` (e.g. with `Set_Position()` and `Get_Name()`) actually correct?**
  _`Update_Position()` has 3 INFERRED edges - model-reasoned connections that need verification._