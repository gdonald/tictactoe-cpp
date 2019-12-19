#include "Board.h"

Board::~Board() {}

Board::Board() {
  for (int row = 0; row < SIZE; row++)
    for (int col = 0; col < SIZE; col++)
      moves[row][col] = EMPTY;
}

std::vector<Move> Board::legalMoves() {
  std::vector<Move> v;

  for (int r = 0; r < SIZE; r++)
    for (int c = 0; c < SIZE; c++)
      if (legalMove(c, r))
        v.emplace_back(c, r);

  return v;
}

bool Board::legalMove(int col, int row) {
  return moves[row][col] == EMPTY;
}

Board::Board(Board const &board) {
  for (int row = 0; row < SIZE; row++)
    for (int col = 0; col < SIZE; col++)
      moves[row][col] = board.moves[row][col];
}

int Board::getXO(int col, int row) {
  return moves[row][col];
}

void Board::addMove(const Move &move, int xo) {
  if (moves[move.row][move.col] != EMPTY) {
    std::cout << "Cannot add move to occupied: col: " << move.col << ", row: " << move.row << std::endl;
    throw;
  }

  moves[move.row][move.col] = xo;
}

int Board::getWinner() {
  for (auto col = 0; col < 3; col++)
    if (moves[0][col] != EMPTY && moves[0][col] == moves[1][col] &&
        moves[0][col] == moves[2][col]) { return moves[0][col]; }

  for (auto row = 0; row < 3; row++)
    if (moves[row][0] != EMPTY && moves[row][0] == moves[row][1] &&
        moves[row][0] == moves[row][2]) { return moves[row][0]; }

  if (moves[0][0] != EMPTY && moves[0][0] == moves[1][1] && moves[0][0] == moves[2][2]) { return moves[0][0]; }
  if (moves[2][0] != EMPTY && moves[2][0] == moves[1][1] && moves[2][0] == moves[0][2]) { return moves[2][0]; }

  return EMPTY;
}
