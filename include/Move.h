#ifndef MOVE_H
#define MOVE_H

class Move {
public:
  Move(int col, int row);

  explicit Move(Move *move);

  int col{};
  int row{};
};

#endif
