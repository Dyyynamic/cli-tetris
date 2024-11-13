#pragma once

class GameController;  // Forward declaration

class GameState
{
  protected:
    GameController *controller;

  public:
    GameState(GameController *controller);

    virtual void update(int ch) = 0;
    virtual void draw() = 0;
};
