#pragma once

#include <curses.h>

#include <string>

#include "GameState.h"

class GameOverState : public GameState
{
  private:
    int width;
    int height;
    int y;
    int x;

    WINDOW* mainWin;
    WINDOW* scoreWin;

    int score;
    int lines;
    int level;

    // bool isTyping;

    std::string name;

  public:
    GameOverState(GameController* controller, int score, int lines, int level);

    // void getUserName();

    void update(int ch);
    void draw();
};
