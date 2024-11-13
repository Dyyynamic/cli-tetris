#include "GameController.h"

#include <curses.h>

#include "MainMenuState.h"

GameController::GameController() : state(new MainMenuState(this)), running(true)
{
}

void GameController::changeState(GameState *newState)
{
  clear();
  state = newState;
}
void GameController::update(int ch) { state->update(ch); }
void GameController::draw() { state->draw(); }

void GameController::quit() { running = false; }
bool GameController::isRunning() { return running; }
