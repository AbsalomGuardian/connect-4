#include "Connect4.h"
#include "Logger.h"
using namespace std;
//starting from a copy of tictactoe.cpp
//probably going to have to do the same steps to include my logging system into this code, but don't do anything so
//structural until I can build it
Connect4::Connect4()
{
    _grid = new Grid(7, 6);
    _gameOptions.AIMAXDepth = 3;
}

Connect4::~Connect4()
{
    delete _grid;
}

//
// make an X or an O
//
Bit* Connect4::PieceForPlayer(const int playerNumber)
{
    // depending on playerNumber load the "red.png" or the "yellow.png" graphic
    Bit *bit = new Bit();
    // should possibly be cached from player class?
    bit->LoadTextureFromFile(playerNumber == AI_PLAYER ? "red.png" : "yellow.png");
    bit->setOwner(getPlayerAt(playerNumber == AI_PLAYER ? 1 : 0));
    return bit;
}

void Connect4::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = 7;
    _gameOptions.rowY = 6;
    _grid->initializeSquares(80, "square.png");

    if (gameHasAI()) {
        setAIPlayer(AI_PLAYER);
    }

    startGame();
}


//find the cordinates of the lowest empty space in the given column
//lowest means closest to the bottom which actually means greatest index
//just returns the y of that place
int Connect4::findLowest(int x) {
    //printf("findLowest called\n");
    for(int i = 5; i >= 0; i--) {
        //printf("looking at row %d\n", i);
        int index = (i * 7) + x;
        //printf("looking at index %d\n", index);
        if(ownerAt((i * 7) + x) == nullptr) {
            
            return i;
        }
    }
    //exception catch case that I don't see how it should ever happen
    Logger* L = Logger::getInstance();
    L->LogError("findLowest() failed to find a valid result.");
    return 0;
}

//overload of findLowest that uses state strings isntead
//this findLowest does expect to find a column that doesn't have a lowest, returns NULL then
int Connect4::findLowest(int x, const string &s) {
    for(int i = 5; i >= 0; i--) {
        //printf("looking at row %d\n", i);
        int index = (i * 7) + x;
        //printf("looking at index %d\n", index);
        if(s[(i * 7) + x] == '0') {
            return i;
        }
    }
    return NULL;

}


bool Connect4::actionForEmptyHolder(BitHolder &holder) //hopefully i can just change it to ChessSquare
//took changing Game.h and Game.cpp, but as ChessSquare is a child and what is actually used to create the grid
//everything should work
{
    ChessSquare* square = dynamic_cast<ChessSquare*>(&holder);
    //printf("!!!You clicked on x: %d y: %d\n!!!", square->getColumn(), square->getRow());
    int x = square->getColumn();
    //printf("checking that column is free\n");
    if (ownerAt(x) != nullptr) { //check instead for holder being empty, for the top row being empty
        return false;
    }
    Bit *bit = PieceForPlayer(getCurrentPlayer()->playerNumber() == 0 ? HUMAN_PLAYER : AI_PLAYER);
    if (bit) {
        /*//start at top of column
        ChessSquare *s = _grid->getSquare(x, 0); //get square at top of colum
        ImVec2 pos = s->getPosition(); //get that square's graphical position
        //place bit at the top
        bit->setPosition(pos); */ 
        //the animate function in Game.h is a virtual
        //move to bottom of the column
        int targetY = findLowest(x);
        //repeat same process of getting the graphical position
        ChessSquare *finalplace = _grid->getSquare(x, targetY); //get square at top of colum
        finalplace->setBit(bit);
        ImVec2 pos = finalplace->getPosition();
        //printf("final bit placement at x: %d, y: %d\n", x, targetY);
        bit->setPosition(pos); //trusting moveTo handles all of the reassignment. it does not
        endTurn();
        return true;
    }   
    return false;
}

bool Connect4::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    // you can't move anything in connect4
    return false;
}

bool Connect4::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst)
{
    // you can't move anything in connect4
    return false;
}

//
// free all the memory used by the game on the heap
//
void Connect4::stopGame()
{
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
}

//
// helper function for the winner check
//
Player* Connect4::ownerAt(int index ) const
{

    int x = index % 7;
    int y = index / 7;
    //printf("From ownerAt, looking at x: %d y: %d\n", x, y);
    auto square = _grid->getSquare(x, y); //have to change what these numbers are  
    if (!square || !square->bit()) {
        return nullptr;
    }
    return square->bit()->getOwner();
}

//version for use with simGrid
Player* Connect4::ownerAt(int index, Grid *grid) const
{

    int x = index % 7;
    int y = index / 7;
    //printf("From ownerAt, looking at x: %d y: %d\n", x, y);
    auto square = grid->getSquare(x, y); //have to change what these numbers are  
    if (!square || !square->bit()) {
        return nullptr;
    }
    return square->bit()->getOwner();
}

//check for 4 in a row in the specified direction, does so recursively
//grid must be specified so it can use either the real grid or the simulation grid in the AI
//Direction index system: 0 = north; 1 = east; 2 = south; 3 = west; 4 = up-left; 5 = up-right; 6 = bottom-right; 7 = bottom-left
//returns the amount found in a row for use by AI
int Connect4::checkDirection(ChessSquare* square, Player* owner, int index, int counter, int direction, Grid *grid) {
    Logger* L = Logger::getInstance();
    L->LogInfo("Called by index " + to_string(index) + " starting with counter of " + to_string(counter) + 
    " to check direction " + to_string(direction));
    //call from outside should start with counter = 1.
    int x = square->getColumn();
    int y = square->getRow();
    //i'm afraid to mess up using switch, so you get this instead
    ChessSquare *next = nullptr;
    if(direction == 0) {next = grid->getN(x, y); }
    else if(direction == 1) {next = grid->getE(x, y);}
    else if(direction == 2) {next = grid->getS(x, y);}
    else if(direction == 3) {next = grid->getW(x, y);}
    else if(direction == 4) {next = grid->getFL(x, y);}
    else if(direction == 5) {next = grid->getFR(x, y);}
    else if(direction == 6) {next = grid->getBR(x, y);}
    else if(direction == 7) {next = grid->getBL(x, y);}
    
    if(next != nullptr) { //does n exist
        int nextIndex = next->getSquareIndex();
        //L->LogInfo("next does exist and is at index " + to_string(nextIndex));
        if(ownerAt(nextIndex, grid) == owner) { //is n held by owner
            //L->LogInfo("next is held by the same player as origin");
            counter++;
            L->LogGameEvent("counter has become " + to_string(counter));
            if(counter == 4) { //has a four in a row been found. base case for the recursion
                return 4;
            }
            //call check direction on next
            int result = checkDirection(next, owner, next->getSquareIndex(), counter, direction, grid);
            L->LogGameEvent("result returned up recursion is " + to_string(result)); 
            if(result == 4) {
                return 4;
            }
            //pass result up the call stack. a call either goes deeper, eventually reaching true or returns false
            return result;
        } else {
            L->LogInfo("next is held by a different player");
            return counter;
        }
    } else {
        L->LogInfo("next does not exist.");
        return counter;
    }
}

//check direction done based on state string
//owner is the number in the actual state string
int Connect4::checkDirectionString(int index, int counter, int owner, int direction, const string &s) {
    Logger *L = Logger::getInstance();
    int x = index % 7;
    int y = index / 7;

    int next = NULL; //next is the index of the place found
    if(direction == 0) {
        //check north
    } else if(direction == 1) {
        //check eat
    }else if(direction == 2) {
        //check south
    }else if(direction == 3) {
        //check west
    } else if(direction == 4) {
        //check north west

    } else if(direction == 5) {
        //check north east
    }else if(direction == 6) {
        //check south east
    }else if(direction == 7) {
        //check south east
    }

}






//returns pointer to the winning player, otherwise returns nullptr
Player* Connect4::checkForWinner()
{
    Logger* L = Logger::getInstance();
    //L->LogGameEvent("Checking for winner...");
    //L->LogGameEvent("***************");
    Player *winner = nullptr; //iterator prevents me from just returning from within
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = y * 7 + x; //index of current square
        Player *owner = ownerAt(index); //owner of current square
        int counter = 1;
        if(owner != nullptr) { //if square has piece, check all its neighbors
            //L->LogInfo("Checking square x: " + to_string(x) + " y: " + to_string(y) + " index: " + to_string(index));
            //L->LogInfo("********************");
            //L->LogInfo("Square is owned by " + to_string(owner->playerNumber()));
            for(int i = 0; i < 8; i++) { //check all directions
                //L->LogInfo("checking direction " + to_string(i));
                //L->LogInfo("-------------------");
                int result = checkDirection(square, owner, index, counter, i, _grid);
                //L->LogInfo("result of that check was: " + to_string(result));
                if(result == 4) { //was 4 in a row found
                    //L->LogGameEvent("Player " + to_string(owner->playerNumber()) + " has won.");
                    winner = owner;
                    break;
                    
                }
            }
        }
    });
    return winner;
    
}

bool Connect4::checkForDraw() //only checks for full, not no possible victory no matter what
{
    //since I'm not certain this will only be called when the board is the same as the state string,
    //I'll just stick with this version of the method
    bool isDraw = true;
    // check to see if the board is full
    _grid->forEachSquare([&isDraw](ChessSquare* square, int x, int y) {
        if (!square->bit()) {
            isDraw = false;
        }
    });
    return isDraw;
}

//
// state strings
//
std::string Connect4::initialStateString() //need to make state string longer
{
    //7*6 = 42
    return "000000000000000000000000000000000000000000";
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string Connect4::stateString()
{
    //modifies the specific bytes of the string instead of creating a new one
    std::string s = "000000000000000000000000000000000000000000";
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        Bit *bit = square->bit();
        if (bit) {
            s[y * 7 + x] = std::to_string(bit->getOwner()->playerNumber()+1)[0];
        }
    });
    return s;
}

//
// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void Connect4::setStateString(const std::string &s)
{
    Logger *L = Logger::getInstance();
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = y* 7 + x;
        //changed to be certain player number is correct
        int playerNumber = (int) s[index];
        if (playerNumber == 1) {
            square->setBit( PieceForPlayer(-1));
        } else if(playerNumber == 2) {
            square->setBit(PieceForPlayer(1));
        } else {
            square->setBit( nullptr );
        }
    });
}

//overload of setStateString with specified grid
void Connect4::setStateString(const std::string &s, Grid* grid)
{
    Logger *L = Logger::getInstance();
    //L->LogInfo("setting grid with string " + s);
    grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = y* 7 + x;
        //changed to be certain player number is correct
        int playerNumber = ((int) s[index]) - '0';
        //L->LogInfo(to_string(playerNumber) + " at index " + to_string(index));
        if (playerNumber == 1) {
            square->setBit( PieceForPlayer(-1));
            //L->LogInfo("simGrid placed 1 at x: " + to_string(x) + " y: " + to_string(y));
        } else if(playerNumber == 2) {
            //L->LogInfo("simGrid placed 2 at x: " + to_string(x) + " y: " + to_string(y));
            square->setBit(PieceForPlayer(1));
        } else {
            square->setBit( nullptr );
        }
    });
}


//return with vector of the rows that is the lowest for the column that = each index. NULL if column is full.
//based on state string, uses overloaded findlowest based on state string
std::vector<int> Connect4::ValidMoves(const std::string &s) {
    vector<int> results;
    for(int i = 0; i < 6; i++) { //here i = column and index of vector
        int lowest = findLowest(i, s);
        results.push_back(lowest);
    }
    return results;
}

//
// this is the function that will be called by the AI
//
void Connect4::updateAI() 
{
    //NEED TO CHANGE TO ONLY LOOK AT VALID MOVES IN NEGAMAX
    int bestVal = -1000;
    BitHolder* bestMove = nullptr;
    std::string state = stateString();
    Logger *L = Logger::getInstance();
    vector<int> moves = ValidMoves(state);
    //instead of traversing all cells, just traverse those with the cordinates [index], value in moves
    for(int i = 0; i < 6; i++) {
        if(moves[i] == NULL) { //skip this column if moves[i]
            continue;
        }
        int index = moves[i] * 7 + i; 
        ChessSquare* square = _grid->getSquareByIndex(index);
        if(HUMAN_PLAYER == -1) { //set 1 or 2 based on who is going first
                state[index] = '2';
        } else {
                state[index] = '1';
        }
            L->LogGameEvent("evaluating x: " + to_string(i) + " y: " + to_string(moves[i]));
            int moveVal = -negamax(state, 0, HUMAN_PLAYER, -1000, 1000);
            // Undo the move
            state[index] = '0';
            // If the value of the current move is more than the best value, update best
            if (moveVal > bestVal) {
                //neet to get square
                bestMove = square;
                bestVal = moveVal;
            }
    }
    
    

    // Make the best move
    if(bestMove) {
        if(actionForEmptyHolder(*bestMove)) {
        }
    }
}

//not sure this same simplicity can be done to check for draw
//added to class because TicTacToe has a method of the same name
bool Connect4::isAIBoardFull(const std::string& state) {
    return state.find('0') == std::string::npos;
}

//this function doesn't check for blocks, but I was told this wasn't nessecary to have better than random results
int Connect4::evaluateAIBoard(const std::string& state, int playerColor) { //have to make member method to access the other methods
    //create simulation grid
    //change check direction to return how far they got before running out
    //since only doing one direction, double counting won't happen, because it has to be the first one
    //will double count partial strings, but do to orders of magnitude this should hopefully not cause too many problems
    //2: +10; 3: +100; 4: +1000 and return
    //grid will also now need to be passed for these functions
    //variables for keeping track of score
    int score = 0;
    Logger *L = Logger::getInstance();
    L->LogInfo("evaluating board with string " + state);
    //playerColor should be -1/1 and should correspond to the results of getPlayerAt
    //playerColor doesn't seem to correspond to player number, but I can derive that
    int playerNum;
    int opponentNum;
    if(playerColor == HUMAN_PLAYER) {
        playerNum = getHumanPlayer();
        opponentNum = getAIPlayer();
    } else {
        playerNum = getAIPlayer();
        opponentNum = getHumanPlayer();
    }
    Player *self = getPlayerAt(playerNum);
    Player *opponent = getPlayerAt(opponentNum);
    //create simGrid and initalize it
    Grid *simGrid = new Grid(7,6);
    setStateString(state, simGrid); //sets grid to look like state
    //traverse simGrid
    simGrid->forEachSquare([&](ChessSquare* square, int x, int y) {
        
        int index = y * 7 + x; //index of current square
        Player *owner = ownerAt(index, simGrid); //owner of current square
        int counter = 1;
        if(owner != nullptr) { //if square has piece, check all its neighbors
            L->LogInfo("evaluating x: " + to_string(x) + " y: " + to_string(y));
            L->LogInfo("owner is " + to_string(owner->playerNumber()));
            for(int i = 0; i < 8; i++) { //check all directions
                int result = checkDirection(square, owner, index, counter, i, simGrid);
                L->LogError("result of check " + to_string(i) + ": " + to_string(result));
                if(owner == self) {
                    if(result == 4) {
                        score = 1000; //if there's a victory, don't need to bother with rest of the grid
                        break; //use break because simGrid still needs to be destoryed
                    } else if(result == 3) {
                        score = score + 100;
                    } else if(result == 2) {
                        score = score + 10;
                    }
                } else { //if opponents in a rows were being calculated
                    if(result == 4) {
                        score = -1000; //if there is a loss, no need to bother with rest of the grid
                        break;
                    } else if(result == 3) {
                        score = score + 100;
                    } else if(result == 2) {
                        score = score + 10;
                    }

                }
            }
        }
    });


    //destroy simGrid after use
    simGrid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
    delete(simGrid);
    
    //finally return score
    return score;
}

//
// player is the current player's number (AI or human)
//
int Connect4::negamax(std::string& state, int depth, int playerColor, int alpha, int beta) 
{
    Logger *L = Logger::getInstance();
    L->LogInfo("negamax called at depth " + to_string(depth) + " with color " + to_string(playerColor));
    if(isAIBoardFull(state)) { //check for draw first, as eval with give a non-zero value for a draw
        return 0; // Draw
    }

    int score = evaluateAIBoard(state, playerColor);
    L->LogInfo("score is " + to_string(score));
    //max depth is 8
    //Caller has the opposite score from that of the playerColor based to eval
    if(depth >= _gameOptions.AIMAXDepth) {
        L->LogInfo("Returning score because max depth has been reached");
        return score;
    }
    if(score == 1000) {
        L->LogInfo("returning score because victory (for calle) terminal state has been reached");
        return score;
    }
    if(score == -1000) {
        L->LogInfo("returning score because loss (for calle) terminal state has been reached");
        return score;
    }

    if(score == 0) { //my own kind of pruning, unsure if its a good idea
        L->LogInfo("aborting path because either everything is perfectly balanced or there's not enough information");
        return score;
    }

    int bestVal = -1000; // Min value. repersents a loss
    //only need to evaluate per free column
    vector<int> moves = ValidMoves(state);
    //instead of traversing all cells, just traverse those with the cordinates [index], value in moves
    for(int i = 0; i < 6; i++) {
        if(moves[i] == NULL) { //skip this column if moves[i]
            continue;
        }
        int index = moves[i] * 7 + i; 
        L->LogInfo("Evaluating index " + to_string(index));
        ChessSquare* square = _grid->getSquareByIndex(index);
        //set the cell to the current player's color
        if(playerColor == 1) {
            state[index] = 2;
        } else {
            state[index] = 1;
        }
        score = -negamax(state, depth+1, -playerColor, -beta, -alpha); //-negamax takes care of score inversion
        //switch alpha and beta when recursing
        L->LogGameEvent("Now back on depth " + to_string(depth) + " with color " + to_string(playerColor));
        //alpha beta pruning
        if(score > bestVal) {
            bestVal = score;
            if(score > alpha) {
                alpha = score;
            }
        }
        if(score >= beta) {
            L->LogInfo("pruning has occured");
            state[i] = '0';
            return bestVal;
        }
        // Undo the move for backtracking
        state[i] = '0';
    }

    return bestVal;
}