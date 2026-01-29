#include <iostream>
#include <string>
#include "../include/Board.h"

using namespace std;

int main()
{
    Board chessGame;
    
    cout << "=== CASTLING TEST CASES ===\n\n";
    
    // TEST 1: Clear path for castling - White Kingside
    cout << "TEST 1: White Kingside castling (path clear)\n";
    string whiteKingside = "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1";
    chessGame.Set_Up_Board(whiteKingside);
    chessGame.Display();
    cout << "White Kingside Castling (e1 -> g1):\n";
    chessGame.Execute_Move(7, 4, 7, 6);  // King e1 -> g1
    chessGame.Display();
    cout << "\n";
    
    // TEST 2: White Queenside castling
    cout << "TEST 2: White Queenside castling (path clear)\n";
    string whiteQueenside = "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1";
    chessGame.Set_Up_Board(whiteQueenside);
    chessGame.Display();
    cout << "White Queenside Castling (e1 -> c1):\n";
    chessGame.Execute_Move(7, 4, 7, 2);  // King e1 -> c1
    chessGame.Display();
    cout << "\n";
    
    // TEST 3: Black castling
    cout << "TEST 3: Black Kingside castling\n";
    string blackKingside = "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R b kq - 0 1";
    chessGame.Set_Up_Board(blackKingside);
    chessGame.Display();
    cout << "Black Kingside Castling (e8 -> g8):\n";
    chessGame.Execute_Move(0, 4, 0, 6);  // King e8 -> g8
    chessGame.Display();
    cout << "\n";
    
    // TEST 4: No castling rights
    cout << "TEST 4: No castling rights (should fail)\n";
    string noCastling = "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w - - 0 1";
    chessGame.Set_Up_Board(noCastling);
    chessGame.Display();
    cout << "Attempting White Kingside (should fail):\n";
    chessGame.Execute_Move(7, 4, 7, 6);  // Should fail
    chessGame.Display();
    cout << "\n";
    
    // TEST 5: Path blocked - Kingside
    cout << "TEST 5: Path blocked by piece (should fail)\n";
    string blockedKingside = "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K1NR w KQkq - 0 1";
    chessGame.Set_Up_Board(blockedKingside);
    chessGame.Display();
    cout << "Attempting White Kingside with Knight at f1 (should fail):\n";
    chessGame.Execute_Move(7, 4, 7, 6);  // Should fail - knight at f1
    chessGame.Display();
    cout << "\n";
    
    // TEST 6: Path blocked - Queenside
    cout << "TEST 6: Queenside path blocked (should fail)\n";
    string blockedQueenside = "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/RN2K2R w KQkq - 0 1";
    chessGame.Set_Up_Board(blockedQueenside);
    chessGame.Display();
    cout << "Attempting White Queenside with Knight at b1 (should fail):\n";
    chessGame.Execute_Move(7, 4, 7, 2);  // Should fail - knight at b1
    chessGame.Display();
    cout << "\n";
    
    // TEST 7: King has moved (test tracking)
    cout << "TEST 7: Testing King movement tracking\n";
    string trackTest = "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1";
    chessGame.Set_Up_Board(trackTest);
    chessGame.Display();
    cout << "Moving King e1 -> f1 (should disable castling):\n";
    chessGame.Execute_Move(7, 4, 7, 5);  // King moves to f1
    chessGame.Display();
    cout << "Moving King back f1 -> e1:\n";
    chessGame.Execute_Move(7, 5, 7, 4);  // King back to e1
    chessGame.Display();
    cout << "Now attempting castling (should fail - king moved):\n";
    chessGame.Execute_Move(7, 4, 7, 6);  // Should fail
    chessGame.Display();
    cout << "\n";
    
    // TEST 8: Rook has moved
    cout << "TEST 8: Rook movement tracking\n";
    string rookTest = "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1";
    chessGame.Set_Up_Board(rookTest);
    chessGame.Display();
    cout << "Moving Kingside Rook h1 -> h2:\n";
    chessGame.Execute_Move(7, 7, 6, 7);  // Rook h1 -> h2
    chessGame.Display();
    cout << "Moving Rook back h2 -> h1:\n";
    chessGame.Execute_Move(6, 7, 7, 7);  // Rook back to h1
    chessGame.Display();
    cout << "Attempting Kingside castling (should fail - rook moved):\n";
    chessGame.Execute_Move(7, 4, 7, 6);  // Should fail
    chessGame.Display();
    cout << "\n";
    
    // TEST 9: Only some castling rights
    cout << "TEST 9: Partial castling rights\n";
    string partialRights = "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w K - 0 1";
    chessGame.Set_Up_Board(partialRights);
    chessGame.Display();
    cout << "White only has Kingside rights, attempting Queenside (should fail):\n";
    chessGame.Execute_Move(7, 4, 7, 2);  // Should fail - no Q rights
    chessGame.Display();
    cout << "Now attempting Kingside (should work):\n";
    chessGame.Execute_Move(7, 4, 7, 6);  // Should work
    chessGame.Display();
    cout << "\n";
    
    cout << "=== CASTLING TESTS COMPLETED ===\n";
    
    return 0;
}