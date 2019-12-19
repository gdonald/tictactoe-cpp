#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <vector>

#include "Move.h"

#define SIZE 3

#define X -1
#define EMPTY 0
#define O 1

class Board {
public:
  Board();

  Board(Board const &board);

  ~Board();

  bool legalMove(int col, int row);

  std::vector<Move> legalMoves();

  int getXO(int col, int row);

  void addMove(const Move &move, int xo);

  int getWinner();

private:
  int moves[SIZE][SIZE]{};
};

#endif
