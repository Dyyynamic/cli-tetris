#pragma once

#include "GameState.h"

class GameController
{
  private:
    GameState *state;
    bool running;

  public:
    GameController();

    void changeState(GameState *newState);
    void update(int ch);
    void draw();

    void quit();
    bool isRunning();
};
