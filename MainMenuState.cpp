#include "MainMenuState.h"

#include "GamePlayState.h"

MainMenuState::MainMenuState(GameController *controller)
    : GameState(controller),
      width(42),
      height(10),
      y((LINES - height) / 2),
      x((COLS - width) / 2),
      mainWin(newwin(height, width, y + 1, x))
{
}

void MainMenuState::update(int ch)
{
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

void MainMenuState::draw()
{
  wclear(mainWin);
  box(mainWin, 0, 0);

  mvwprintw(mainWin, 2, 4, "TETRIS");
  mvwprintw(mainWin, 4, 4, "1. Start");
  mvwprintw(mainWin, 5, 4, "2. Quit");

  mvwprintw(mainWin, 7, 4, "Made by");
  wattron(mainWin, COLOR_PAIR(8));
  mvwprintw(mainWin, 7, 12, "@Dyyynamic");
  wattroff(mainWin, COLOR_PAIR(8));
  mvwprintw(mainWin, 7, 23, "on github");

  wrefresh(mainWin);
}
