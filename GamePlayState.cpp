#include "GamePlayState.h"

#include <algorithm>
#include <random>

#include "MainMenuState.h"
#include "GameOverState.h"

#define O_PIECE 3
#define I_PIECE 6

GamePlayState::GamePlayState(GameController *controller)
    : GameState(controller),
      width(22),
      height(22),
      y((LINES - height) / 2),
      x((COLS - width) / 2),
      fallTime(0),
      fallSpeed(32),
      fastFall(false),
      score(0),
      lines(0),
      level(0),
      top(0),
      linesCleared(0),
      stats(7, 0),
      blockCounter(20, 0),
      mainWin(newwin(height, width, y + 1, x)),
      linesWin(newwin(3, width, y - 2, x)),
      scoreWin(newwin(6, 12, y - 2, x + width + 1)),
      nextWin(newwin(6, 12, y + 4, x + width + 1)),
      levelWin(newwin(4, 12, y + 10, x + width + 1)),
      statsWin(newwin(24, 16, y - 2, x - 17))
{
  pieceShapes = {{
      // T piece: 4 rotations
      {{
          {{{2, 1}, {2, 2}, {2, 3}, {3, 2}}},
          {{{1, 2}, {2, 1}, {2, 2}, {3, 2}}},
          {{{1, 2}, {2, 1}, {2, 2}, {2, 3}}},
          {{{1, 2}, {2, 2}, {2, 3}, {3, 2}}},
      }},
      // J piece: 4 rotations
      {{
          {{{2, 1}, {2, 2}, {2, 3}, {3, 3}}},
          {{{1, 2}, {2, 2}, {3, 1}, {3, 2}}},
          {{{1, 1}, {2, 1}, {2, 2}, {2, 3}}},
          {{{1, 2}, {1, 3}, {2, 2}, {3, 2}}},
      }},
      // Z piece: 2 rotations
      {{
          {{{2, 1}, {2, 2}, {3, 2}, {3, 3}}},
          {{{1, 3}, {2, 2}, {2, 3}, {3, 2}}},
          {{{2, 1}, {2, 2}, {3, 2}, {3, 3}}},  // Repeated
          {{{1, 3}, {2, 2}, {2, 3}, {3, 2}}},  // Repeated
      }},
      // O piece: 1 rotation
      {{
          {{{2, 1}, {2, 2}, {3, 1}, {3, 2}}},
          {{{2, 1}, {2, 2}, {3, 1}, {3, 2}}},  // Repeated
          {{{2, 1}, {2, 2}, {3, 1}, {3, 2}}},  // Repeated
          {{{2, 1}, {2, 2}, {3, 1}, {3, 2}}},  // Repeated
      }},
      // S piece: 2 rotations
      {{
          {{{2, 2}, {2, 3}, {3, 1}, {3, 2}}},
          {{{1, 2}, {2, 2}, {2, 3}, {3, 3}}},
          {{{2, 2}, {2, 3}, {3, 1}, {3, 2}}},  // Repeated
          {{{1, 2}, {2, 2}, {2, 3}, {3, 3}}},  // Repeated
      }},
      // L piece: 4 rotations
      {{
          {{{2, 1}, {2, 2}, {2, 3}, {3, 1}}},
          {{{1, 1}, {1, 2}, {2, 2}, {3, 2}}},
          {{{1, 3}, {2, 1}, {2, 2}, {2, 3}}},
          {{{1, 2}, {2, 2}, {3, 2}, {3, 3}}},
      }},
      // I piece: 2 rotations
      {{
          {{{2, 0}, {2, 1}, {2, 2}, {2, 3}}},
          {{{0, 2}, {1, 2}, {2, 2}, {3, 2}}},
          {{{2, 0}, {2, 1}, {2, 2}, {2, 3}}},  // Repeated
          {{{0, 2}, {1, 2}, {2, 2}, {3, 2}}},  // Repeated
      }},
  }};

  currPiece = randomPiece(pieceShapes);
  nextPiece = randomPiece(pieceShapes);

  stats[currPiece.type]++;
  currPiece.pos = {-2, 3};
}

Piece GamePlayState::randomPiece(
    const std::array<std::array<std::array<Point, 4>, 4>, 7> &pieceShapes)
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 6);
  const int randIdx{dis(gen)};

  return {randIdx, {0, 0}, randIdx + 1, pieceShapes[randIdx], 0};
}

void GamePlayState::drawBlock(WINDOW *win, Block block, int offsetX)
{
  wattron(win, COLOR_PAIR(block.color));
  mvwprintw(win, block.pos.y + 1, block.pos.x * 2 + 1 + offsetX, "  ");
  wattroff(win, COLOR_PAIR(block.color));
}

void GamePlayState::drawPiece(WINDOW *win, Piece piece, int offsetX)
{
  for (auto point : piece.shapes[piece.rotation])
  {
    // Transform point to block
    Block block{
        piece.pos.y + point.y,
        piece.pos.x + point.x,
        piece.color,
    };

    // To prevent drawing over the border at the
    // top when rotating the piece
    if (block.pos.y < 0) continue;

    drawBlock(win, block, offsetX);
  }
}

bool GamePlayState::checkCollision(const Piece piece,
                                   const std::vector<Block> &staticBlocks)
{
  for (auto point : piece.shapes[piece.rotation])
  {
    // Collision with wall or floor
    if (piece.pos.x + point.x < 0 || piece.pos.x + point.x >= 10 ||
        piece.pos.y + point.y >= 20)
      return true;

    // Collision with another piece
    for (auto block : staticBlocks)
      if (piece.pos.x + point.x == block.pos.x &&
          piece.pos.y + point.y == block.pos.y)
        return true;
  }

  return false;
}

void clearLine(std::vector<Block> &staticBlocks, int y)
{
  // Remove all points in the line
  staticBlocks.erase(
      std::remove_if(staticBlocks.begin(), staticBlocks.end(),
                     [y](const Block &block) { return block.pos.y == y; }),
      staticBlocks.end());

  // Move all points above the line down
  for (auto &block : staticBlocks)
    if (block.pos.y < y) block.pos.y++;
}

void GamePlayState::update(int ch)
{
  switch (ch)
  {
    case KEY_LEFT:
      currPiece.pos.x--;

      if (checkCollision(currPiece, staticBlocks)) currPiece.pos.x++;

      break;

    case KEY_RIGHT:
      currPiece.pos.x++;

      if (checkCollision(currPiece, staticBlocks)) currPiece.pos.x--;

      break;

    case KEY_DOWN:
      fastFall = true;
      break;

    case 'z':
    case 'Z':
      currPiece.rotation = (currPiece.rotation + 1) % 4;

      if (checkCollision(currPiece, staticBlocks))
        currPiece.rotation = (currPiece.rotation + 3) % 4;

      break;

    case 'x':
    case 'X':
      currPiece.rotation = (currPiece.rotation + 3) % 4;

      if (checkCollision(currPiece, staticBlocks))
        currPiece.rotation = (currPiece.rotation + 1) % 4;

      break;

    case ERR:
      fastFall = false;
      break;

    case 'q':
    case 'Q':
      controller->quit();
      break;
  }

  if (++fallTime >= fallSpeed || fastFall)
  {
    fallTime = 0;
    currPiece.pos.y++;

    if (checkCollision(currPiece, staticBlocks))
    {
      currPiece.pos.y--;

      for (auto point : currPiece.shapes[currPiece.rotation])
      {
        // Transform point to block
        Block block{
            currPiece.pos.y + point.y,
            currPiece.pos.x + point.x,
            currPiece.color,
        };
        staticBlocks.push_back(block);
        blockCounter[block.pos.y]++;
      }

      for (int i = 0; i < 20; i++)
      {
        if (blockCounter[i] == 10)
        {
          blockCounter.erase(blockCounter.begin() + i);
          blockCounter.insert(blockCounter.begin(), 0);

          clearLine(staticBlocks, i);
          linesCleared++;

          i--;
        }
      }

      switch (linesCleared)
      {
        case 1:
          score += 40 * (level + 1);
          break;
        case 2:
          score += 100 * (level + 1);
          break;
        case 3:
          score += 300 * (level + 1);
          break;
        case 4:
          score += 1200 * (level + 1);
          break;
      }

      lines += linesCleared;
      linesCleared = 0;
      level = lines / 10;

      // Spawn new piece
      currPiece = nextPiece;
      currPiece.pos = {-2, 3};
      nextPiece = randomPiece(pieceShapes);
      stats[currPiece.type]++;

      // Game over
      if (checkCollision(currPiece, staticBlocks))
      {
        controller->changeState(new GameOverState(controller, score, lines, level));
      }
    }
  }
}

void GamePlayState::draw()
{
  // Lines
  wclear(linesWin);
  box(linesWin, 0, 0);
  mvwprintw(linesWin, 1, 2, "Lines - %03d", lines);
  wrefresh(linesWin);

  // Score
  wclear(scoreWin);
  box(scoreWin, 0, 0);
  mvwprintw(scoreWin, 1, 2, "Top");
  mvwprintw(scoreWin, 2, 2, "%06d", top);
  mvwprintw(scoreWin, 3, 2, "Score");
  mvwprintw(scoreWin, 4, 2, "%06d", score);
  wrefresh(scoreWin);

  // Next piece
  wclear(nextWin);
  box(nextWin, 0, 0);
  mvwprintw(nextWin, 1, 2, "Next");
  drawPiece(nextWin, nextPiece,
            nextPiece.type == O_PIECE || nextPiece.type == I_PIECE ? 1 : 0);
  wrefresh(nextWin);

  // Level
  wclear(levelWin);
  box(levelWin, 0, 0);
  mvwprintw(levelWin, 1, 2, "Level");
  mvwprintw(levelWin, 2, 2, "%d", level);
  wrefresh(levelWin);

  // Statistics
  wclear(statsWin);
  box(statsWin, 0, 0);
  mvwprintw(statsWin, 1, 2, "Statistics");
  for (int i = 0; i < 7; i++)
  {
    const Piece piece{i, {i * 3, 0}, i + 1, pieceShapes[i], 0};
    drawPiece(statsWin, piece, i == O_PIECE || i == I_PIECE ? 1 : 0);
    mvwprintw(statsWin, (i + 1) * 3, 11, "%03d", stats[i]);
  }
  wrefresh(statsWin);

  // Main window
  wclear(mainWin);
  box(mainWin, 0, 0);
  for (auto point : staticBlocks) drawBlock(mainWin, point);
  drawPiece(mainWin, currPiece);
  wrefresh(mainWin);

  mvprintw(y + 14, x + width + 1, "Press 'q'");
  mvprintw(y + 15, x + width + 1, "to quit");
}
