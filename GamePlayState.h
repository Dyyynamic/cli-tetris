#pragma once

#include <curses.h>

#include <vector>

#include "Block.h"
#include "GameController.h"
#include "GameState.h"
#include "Piece.h"

class GamePlayState : public GameState
{
  private:
    int width;
    int height;
    int y;
    int x;

    int fallTime;   // How many frames have passed
    int fallSpeed;  // Frames between each fall
    bool fastFall;

    int score;
    int lines;
    int level;

    int top;

    int linesCleared;  // Per piece dropped

    std::array<std::array<std::array<Point, 4>, 4>, 7> pieceShapes;

    std::vector<int> stats;

    Piece currPiece;
    Piece nextPiece;

    std::vector<Block> staticBlocks;  // Pieces that have landed

    std::vector<int> blockCounter;  // Track number of blocks in each row

    WINDOW *mainWin;
    WINDOW *linesWin;
    WINDOW *scoreWin;
    WINDOW *nextWin;
    WINDOW *levelWin;
    WINDOW *statsWin;

    Piece randomPiece(
        const std::array<std::array<std::array<Point, 4>, 4>, 7> &pieceShapes);
    void drawBlock(WINDOW *win, Block block, int offsetX = 0);
    void drawPiece(WINDOW *win, Piece piece, int offsetX = 0);
    bool checkCollision(const Piece piece,
                        const std::vector<Block> &staticBlocks);

  public:
    GamePlayState(GameController *controller);

    void update(int ch) override;
    void draw() override;
};
