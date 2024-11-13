#pragma once

#include <ncurses.h>

#include "GameController.h"

class MainMenuState : public GameState
{
  private:
    int width;
    int height;
    int y;
    int x;

    WINDOW *mainWin;

  public:
    MainMenuState(GameController *controller);

    void update(int ch) override;
    void draw() override;
};
