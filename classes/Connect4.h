#pragma once
#include "Game.h"

//same methods as in TicTacToe.h
class Connect4 : public Game {
public:
    Connect4();
    ~Connect4();

    // set up the board
    void        setUpBoard() override;
    int         checkDirection(ChessSquare* square, Player* owner, int index, int counter, int direction, Grid *grid);
    int         checkDirectionString(int index, int counter, int owner, int direction, const string &s);
    Player*     checkForWinner() override;
    bool        checkForDraw() override;
    std::string initialStateString() override;
    std::string stateString() override;
    void        setStateString(const std::string &s) override;
    void        setStateString(const std::string &s, Grid* grid);
    int         findLowest(int x);
    int         findLowest(int x, const std::string &s);
    bool        actionForEmptyHolder(BitHolder &holder) override;
    bool        canBitMoveFrom(Bit &bit, BitHolder &src) override;
    bool        canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;
    void        stopGame() override;
    std::vector<int> ValidMoves(const std::string &s);
	void        updateAI() override;
    bool        gameHasAI() override { return true; }
    Grid* getGrid() override { return _grid; }
private:
    Bit *       PieceForPlayer(const int playerNumber);
    Player*     ownerAt(int index ) const;
    Player*     ownerAt(int index, Grid *grid) const;
    bool        isAIBoardFull(const std::string& state);
    int         evaluateAIBoard(const std::string& state, int playerColor);
    int         negamax(std::string& state, int depth, int playerColor, int alpha, int beta);

    Grid*       _grid;

};