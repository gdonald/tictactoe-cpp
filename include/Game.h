#ifndef CPP_GAME_H
#define CPP_GAME_H

#include <iostream>
#include <sstream>
#include <thread>

#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>

#include "Board.h"

#define DEPTH 7

#define SCREEN_W  600
#define SCREEN_H 600
#define BOARD_HW  600
#define XO (BOARD_HW / SIZE)

#define BTN_W 120
#define BTN_H 40
#define BTN_SPACE 20

#define FONT "res/font/WickedGrit.ttf"

enum Buttons {
  BtnYes, BtnNo,
  BtnCount
};

enum ButtonStates {
  BtnUp = 0
};

enum Menus {
  MenuNone, MenuGameOver
};

class Game {
public:
  ~Game();

  explicit Game(const char *title);

  bool isRunning();

  void handleEvents();

  void clean();

  void render();

  void drawGrid();

  void drawX(Sint16 col, Sint16 row);

  void drawO(Sint16 col, Sint16 row);

  void drawXOs();

  void drawMenu();

  void drawGameOverMenu();

  void newGame();

  void handleClick(SDL_MouseButtonEvent *event);

  static bool insideRect(SDL_Rect rect, int x, int y);

  bool isPlayerTurn();

  void switchTurn();

  void aiTurn();

  static void aiThread(Game *game);

  Move getAiMove();

  static int minimax(Board *board, int depth, int alpha, int beta, bool maximizingPlayer);

  static int evaluate(Board *board, int xo);

  static bool gameOver(Board *board);

  void writeText(const char *text, int x, int y, TTF_Font *font, SDL_Color color);

private:
  bool running = false;
  int currentMenu = MenuNone;

  SDL_Window *window{};
  SDL_Renderer *renderer{};
  SDL_Surface *bgSurface;
  SDL_Texture *bgTexture;
  Board *board{};

  int mouseX{};
  int mouseY{};

  int turn{};

  SDL_Texture *btnTextures[BtnCount]{};
  SDL_Rect btnRects[BtnCount]{};

  SDL_Surface *btnYesSurface;
  SDL_Surface *btnNoSurface;

  TTF_Font *boardFont;
  TTF_Font *font21;
};

#endif
