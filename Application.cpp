#include "Application.h"
#include "imgui/imgui.h"
#include "classes/TicTacToe.h"
#include "classes/Checkers.h"
#include "classes/Othello.h"
#include "classes/Connect4.h"
#include "classes/Logger.h"
namespace ClassGame {
        //
        // our global variables
        //
        Game *game = nullptr;
        bool gameOver = false;
        int gameWinner = -1;

        //
        // game starting point
        // this is called by the main render loop in main.cpp
        //
        Logger* logger = Logger::getInstance();
        void GameStartUp() 
        {
            game = nullptr;
        }

        //
        // game render loop
        // this is called by the main render loop in main.cpp
        //
        void RenderGame() 
        {
                ImGui::DockSpaceOverViewport();

                //ImGui::ShowDemoWindow();
                logger->ShowLogWindow();
                ImGui::Begin("Settings");
                if (gameOver) {
                    ImGui::Text("Game Over!");
                    ImGui::Text("Winner: %d", gameWinner);
                    if (ImGui::Button("Reset Game")) {
                        game->stopGame();
                        game->setUpBoard();
                        gameOver = false;
                        gameWinner = -1;
                    }
                }
                if (!game) {
                    if (ImGui::Button("Start Tic-Tac-Toe")) {
                        game = new TicTacToe();
                        game->setUpBoard();
                    }
                    if (ImGui::Button("Start Checkers")) {
                        game = new Checkers();
                        game->setUpBoard();
                    }
                    if (ImGui::Button("Start Othello")) {
                        game = new Othello();
                        game->setUpBoard();
                    }
                    if (ImGui::Button("Start Connect 4 AI First")) {
                        logger->LogGameEvent("Would you like to play a game?");
                        game = new Connect4();
                        game->setUpBoard();
                        game->setAIPlayer(0);
                    }
                    if (ImGui::Button("Start Connect 4 AI Second")) {
                        logger->LogGameEvent("Would you like to play a game?");
                        game = new Connect4();
                        game->setUpBoard();
                        game->setAIPlayer(1);
                    }
                } else {
                    ImGui::Text("Current Player Number: %d", game->getCurrentPlayer()->playerNumber());
                    ImGui::Text("Current Board State: %s", game->stateString().c_str());
                }
                ImGui::End();

                ImGui::Begin("GameWindow");
                if (game) {
                    Player *test = game->getCurrentPlayer();
                    bool test2 = game->getCurrentPlayer()->isAIPlayer();
                    //i don't think the game knows how to get who the player is
                    if (game->gameHasAI() && (game->getCurrentPlayer()->isAIPlayer() || game->_gameOptions.AIvsAI))
                    {
                        game->updateAI();
                    }
                    game->drawFrame();
                }
                ImGui::End();
        }

        //
        // end turn is called by the game code at the end of each turn
        // this is where we check for a winner
        //
        void EndOfTurn() 
        {
            Player *winner = game->checkForWinner();
            //printf("WARNING: you have commented out checkforWinner and checkForDraw in application.cpp\n");
            //Player *winner = nullptr;
            if (winner)
            {
                gameOver = true;
                gameWinner = winner->playerNumber();
                if(winner->isAIPlayer()) {
                    logger->LogGameEvent("I have won.");
                } else {
                    logger->LogGameEvent("Opponent has won.");
                }
            }
            if (game->checkForDraw()) {
                gameOver = true;
                gameWinner = -1;
                logger->LogGameEvent("I see we have played to a draw.");
            }
        }
}
