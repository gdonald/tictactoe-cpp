#include "Game.h"

Game::~Game() {
  TTF_CloseFont(boardFont);
  TTF_CloseFont(font21);
  TTF_Quit();

  SDL_DestroyTexture(btnTextures[BtnYes]);
  SDL_DestroyTexture(btnTextures[BtnNo]);

  SDL_DestroyTexture(bgTexture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

Game::Game(const char *title) {
  if (SDL_Init(SDL_INIT_EVERYTHING)) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            SCREEN_W, SCREEN_H, SDL_WINDOW_OPENGL);

  if (!window) {
    printf("Could not create window: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  SDL_SetWindowMinimumSize(window, SCREEN_W, SCREEN_H);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  if (!renderer) {
    printf("Count not get renderer! SDL Error: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

  bgSurface = SDL_LoadBMP("res/img/bg.bmp");
  if (bgSurface == nullptr) {
    printf("Unable to load image %s! SDL Error: %s\n", "res/img/bg.bmp", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
  SDL_FreeSurface(bgSurface);

  btnYesSurface = SDL_LoadBMP("res/img/btn_yes.bmp");
  btnNoSurface = SDL_LoadBMP("res/img/btn_no.bmp");

  if (btnYesSurface == nullptr) {
    printf("Unable to load image %s! SDL Error: %s\n", "res/img/btn_yes.bmp", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  if (btnNoSurface == nullptr) {
    printf("Unable to load image %s! SDL Error: %s\n", "res/img/btn_no.bmp", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  btnTextures[BtnYes] = SDL_CreateTextureFromSurface(renderer, btnYesSurface);
  btnTextures[BtnNo] = SDL_CreateTextureFromSurface(renderer, btnNoSurface);

  SDL_FreeSurface(btnYesSurface);
  SDL_FreeSurface(btnNoSurface);

  if (TTF_Init() == -1) {
    printf("TTF_Init failed: %s\n", TTF_GetError());
    exit(EXIT_FAILURE);
  }

  boardFont = TTF_OpenFont(FONT, 212);
  if (boardFont == nullptr) {
    printf("Failed to load boardFont! Error: %s\n", TTF_GetError());
    exit(EXIT_FAILURE);
  }

  font21 = TTF_OpenFont(FONT, 21);
  if (font21 == nullptr) {
    printf("Failed to load font21! Error: %s\n", TTF_GetError());
    exit(EXIT_FAILURE);
  }

  newGame();

  running = true;
}

void Game::handleEvents() {
  SDL_Event event;

  if (SDL_WaitEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        running = false;
        break;
      case SDL_MOUSEBUTTONUP:
        SDL_GetMouseState(&mouseX, &mouseY);
        handleClick(&event.button);
        break;
    }
  }
}

void Game::handleClick(SDL_MouseButtonEvent *event) {
  switch (currentMenu) {
    case MenuGameOver:
      if (insideRect(btnRects[BtnNo], mouseX, mouseY)) {
        currentMenu = MenuNone;
        render();
        return;
      }
      if (insideRect(btnRects[BtnYes], mouseX, mouseY)) {
        currentMenu = MenuNone;
        newGame();
        render();
        return;
      }
      break;
  }

  if (gameOver(board)) {
    currentMenu = MenuGameOver;
    return;
  }

  if (!isPlayerTurn() || event->button != SDL_BUTTON_LEFT) { return; }

  int col = mouseX / XO;
  int row = mouseY / XO;

  if (board->legalMove(col, row)) {
    board->addMove(Move(col, row), X);

    switchTurn();
    render();

    if (gameOver(board)) {
      currentMenu = MenuGameOver;
      return;
    }

    std::thread t{aiThread, this};
    t.join();

    render();
  }
}

bool Game::insideRect(SDL_Rect rect, int x, int y) {
  return x > rect.x &&
         x < rect.x + rect.w &&
         y > rect.y &&
         y < rect.y + rect.h;
}

bool Game::gameOver(Board *board) {
  if (board->legalMoves().empty()) { return true; }
  return board->getWinner() != EMPTY;
}

void Game::aiThread(Game *game) {
  game->aiTurn();
  game->switchTurn();
}

void Game::aiTurn() {
  if (isPlayerTurn())
    return;

  Move m = getAiMove();

  if (m.col > -1 && m.row > -1)
    board->addMove(m, O);
}

Move Game::getAiMove() {
  auto moves = board->legalMoves();
  int eval;
  int maxEval = std::numeric_limits<int>::min();
  Move bestMove = Move(-1, -1);

  for (auto &move : moves) {
    auto childBoard = Board(*board);
    childBoard.addMove(move, O);
    eval = minimax(&childBoard, DEPTH, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), false);
    if (eval > maxEval) {
      maxEval = eval;
      bestMove = move;
    }
  }

  return bestMove;
}

int Game::minimax(Board *board, int depth, int alpha, int beta, bool maximizingPlayer) {
  if (depth == 0 || board->legalMoves().empty() || gameOver(board)) {
    return evaluate(board, maximizingPlayer ? X : O);
  }

  int eval;

  if (maximizingPlayer) {
    int maxEval = std::numeric_limits<int>::min();

    for (auto &move : board->legalMoves()) {
      auto childBoard = Board(*board);
      childBoard.addMove(move, O);
      eval = minimax(&childBoard, depth - 1, alpha, beta, false);
      maxEval = std::max(maxEval, eval);
      alpha = std::max(alpha, eval);
      if (beta <= alpha)
        break;
    }

    return maxEval;

  } else {
    int minEval = std::numeric_limits<int>::max();

    for (auto &move : board->legalMoves()) {
      auto childBoard = Board(*board);
      childBoard.addMove(move, X);
      eval = minimax(&childBoard, depth - 1, alpha, beta, true);
      minEval = std::min(minEval, eval);
      beta = std::min(beta, eval);
      if (beta <= alpha)
        break;
    }

    return minEval;
  }
}

int Game::evaluate(Board *board, int xo) {
  if (board->getWinner() != EMPTY) { return xo == X ? -1 : 1; }

  return 0;
}

void Game::switchTurn() {
  if (!gameOver(board)) {
    turn = isPlayerTurn() ? O : X;
  } else {
    currentMenu = MenuGameOver;
  }
}

bool Game::isPlayerTurn() {
  return turn == X;
}

void Game::render() {
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, bgTexture, nullptr, nullptr);

  drawGrid();
  drawXOs();
  drawMenu();

  SDL_RenderPresent(renderer);
}

void Game::drawMenu() {
  switch (currentMenu) {
    case MenuGameOver:
      drawGameOverMenu();
      break;
  }
}

void Game::drawGameOverMenu() {
  SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xcc);
  SDL_Rect rect;
  rect.x = SCREEN_W / 2 - 160;
  rect.y = SCREEN_W / 2 - 45;
  rect.h = 170;
  rect.w = 320;
  SDL_RenderFillRect(renderer, &rect);

  SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
  SDL_RenderDrawRect(renderer, &rect);

  std::ostringstream result;
  result << "Game Over:  ";

  int winner = board->getWinner();

  if (winner == X) {
    result << "X Wins!";
  } else if (winner == O) {
    result << "O Wins!";
  } else {
    result << "Tie";
  }

  SDL_Color color = {255, 255, 255, 0};
  writeText(result.str().c_str(), 172, 284, font21, color);
  writeText("Play Again?", 172, 325, font21, color);

  SDL_Rect clip[BtnCount];
  clip[BtnNo].x = 0;
  clip[BtnNo].y = BtnUp;
  clip[BtnNo].w = BTN_W;
  clip[BtnNo].h = BTN_H;

  clip[BtnYes].x = 0;
  clip[BtnYes].y = BtnUp;
  clip[BtnYes].w = BTN_W;
  clip[BtnYes].h = BTN_H;

  btnRects[BtnNo].x = (SCREEN_W / 2) - BTN_W - (BTN_SPACE / 2);
  btnRects[BtnNo].y = (int) 370;
  btnRects[BtnNo].w = BTN_W;
  btnRects[BtnNo].h = BTN_H;

  btnRects[BtnYes].x = (SCREEN_W / 2) + (BTN_SPACE / 2);
  btnRects[BtnYes].y = (int) 370;
  btnRects[BtnYes].w = BTN_W;
  btnRects[BtnYes].h = BTN_H;

  SDL_RenderCopy(renderer, btnTextures[BtnNo], &clip[BtnNo], &btnRects[BtnNo]);
  SDL_RenderCopy(renderer, btnTextures[BtnYes], &clip[BtnYes], &btnRects[BtnYes]);
}

bool Game::isRunning() {
  return running;
}

void Game::newGame() {
  board = new Board();
  turn = X;
}

void Game::drawGrid() {
  SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);

  SDL_RenderDrawLine(renderer, 0, 0, 0, BOARD_HW);
  for (int x = 0; x <= BOARD_HW; x += BOARD_HW / SIZE) {
    SDL_RenderDrawLine(renderer, x, 0, x, BOARD_HW);
  }

  SDL_RenderDrawLine(renderer, 0, 0, BOARD_HW, 0);
  for (int y = 0; y <= BOARD_HW; y += BOARD_HW / SIZE) {
    SDL_RenderDrawLine(renderer, 0, y, BOARD_HW, y);
  }
}

void Game::drawXOs() {
  for (Sint16 r = 0; r < SIZE; r++) {
    for (Sint16 c = 0; c < SIZE; c++) {
      int xo = board->getXO(c, r);
      if (xo == X) drawX(c, r);
      else if (xo == O) drawO(c, r);
    }
  }
}

void Game::drawX(Sint16 col, Sint16 row) {
  Sint16 x = (XO * col) + 22;
  Sint16 y = (XO * row) + 24;

  SDL_Color color = {255, 255, 255, 0};
  writeText("X", x, y, boardFont, color);
}

void Game::drawO(Sint16 col, Sint16 row) {
  Sint16 x = (XO * col) + 10;
  Sint16 y = (XO * row) + 22;

  SDL_Color color = {0, 0, 0, 0};
  writeText("O", x, y, boardFont, color);
}

void Game::writeText(const char *text, int x, int y, TTF_Font *font, SDL_Color color) {
  int w, h;

  SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

  TTF_SizeText(font, text, &w, &h);
  SDL_Rect rect = {.x = x, .y = y, .w = w, .h = h};

  SDL_RenderCopy(renderer, texture, nullptr, &rect);

  SDL_DestroyTexture(texture);
  SDL_FreeSurface(surface);
}
