#include <ncurses.h>

#include <array>

#include "GameController.h"
#include "Point.h"

int main()
{
  initscr();
  cbreak();               // Disable line buffering
  noecho();               // Do not display input
  curs_set(0);            // Hide cursor
  keypad(stdscr, TRUE);   // Enable special keys
  nodelay(stdscr, TRUE);  // Make getch non-blocking
  start_color();          // Enable color functionality
  refresh();

  // Colors
  init_pair(1, COLOR_BLACK, COLOR_MAGENTA);  // T piece
  init_pair(2, COLOR_BLACK, COLOR_BLUE);     // J piece
  init_pair(3, COLOR_BLACK, COLOR_RED);      // Z piece
  init_pair(4, COLOR_BLACK, COLOR_YELLOW);   // O piece
  init_pair(5, COLOR_BLACK, COLOR_GREEN);    // S piece
  init_pair(6, COLOR_BLACK, COLOR_WHITE);    // L piece
  init_pair(7, COLOR_BLACK, COLOR_CYAN);     // I piece

  init_pair(8, COLOR_CYAN, COLOR_BLACK);     // Main menu highlight

  GameController controller{};

  int ch;

  while (controller.isRunning())
  {
    ch = getch();

    controller.update(ch);
    controller.draw();

    napms(16);
  }

  endwin();

  return 0;
}
