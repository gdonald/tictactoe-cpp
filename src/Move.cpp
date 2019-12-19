#include "Move.h"

Move::Move(int col, int row) : col(col), row(row) {}

Move::Move(Move *move) : col(move->col), row(move->row) {}
