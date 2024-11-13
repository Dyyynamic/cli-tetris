#include "GameOverState.h"

#include "GamePlayState.h"

GameOverState::GameOverState(GameController* controller, int score, int lines,
                             int level)
    : GameState(controller),
      width(42),
      height(12),
      y((LINES - height) / 2),
      x((COLS - width) / 2),
      mainWin(newwin(height, width, y + 1, x)),
      scoreWin(newwin(4, 22, y + 10, x + 4)),
      score(score),
      lines(lines),
      level(level)
      // isTyping(true)
{
}

// void GameOverState::getUserName()
// {
//   nodelay(stdscr, FALSE);  // Make getch blocking
//   curs_set(1);             // Show cursor

//   int ch;

//   std::string valid{"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
//   std::string paddedName;

//   while (true)
//   {
//     paddedName =
//         name.length() < 6 ? name + std::string(6 - name.length(), '-') : name;

//     mvwprintw(scoreWin, 1, 0, "1 %-6s %06d %03d %02d", paddedName.c_str(),
//               score, lines, level);
//     wmove(scoreWin, 1, name.length() + 2);
//     wrefresh(scoreWin);

//     ch = getch();

//     if (ch == ERR) continue;
//     if (ch == '\n' || ch == KEY_ENTER) break;

//     if (ch == KEY_BACKSPACE && name.length() > 0)
//       name.pop_back();
//     else if (valid.find(toupper(ch)) != std::string::npos && name.length() < 6)
//       name += toupper(ch);
//   }

//   nodelay(stdscr, TRUE);  // Make getch non-blocking
//   curs_set(0);            // Hide cursor

//   isTyping = false;
// }

void GameOverState::update(int ch)
{
  // if (isTyping) return;

  switch (ch)
  {
    case '1':
      controller->changeState(new GamePlayState(controller));
      break;

    case '2':
      controller->quit();
      break;
  }
}

void GameOverState::draw()
{
  wclear(mainWin);
  box(mainWin, 0, 0);

  mvwprintw(mainWin, 2, 4, "GAME OVER");

  mvwprintw(mainWin, 4, 4, "1. Retry");
  mvwprintw(mainWin, 5, 4, "2. Quit");

  mvwprintw(mainWin, 7, 4, "Score: %d", score);
  mvwprintw(mainWin, 8, 4, "Lines: %d", lines);
  mvwprintw(mainWin, 9, 4, "Level: %d", level);

  wrefresh(mainWin);
}
