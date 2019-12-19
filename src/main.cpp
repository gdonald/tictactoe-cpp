#include "Game.h"

auto main() -> int {
  Game *game = new Game("TicTacToe");

  game->render();

  while (game->isRunning()) {
    game->handleEvents();
  }

  game->clean();
  delete game;
}
