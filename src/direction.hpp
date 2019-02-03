#ifndef DIRECTION_HPP
#define DIRECTION_HPP

enum class Direction { N, NE, E, SE, S, SW, W, NW, NONE };
enum class VerticalDirection { UP, DOWN, UP_AND_DOWN, NONE }; // H and V directions are not interchangeble in general

namespace direction {
// TODO deltas larger than 1
static inline Direction getDirectionFromDeltas(int stepDx, int stepDy) {
  Direction direction = Direction::NONE;
  if (stepDx ==  0 && stepDy == -1) direction = Direction::N;
  if (stepDx ==  1 && stepDy == -1) direction = Direction::NE;
  if (stepDx ==  1 && stepDy ==  0) direction = Direction::E;
  if (stepDx ==  1 && stepDy ==  1) direction = Direction::SE;
  if (stepDx ==  0 && stepDy ==  1) direction = Direction::S;
  if (stepDx == -1 && stepDy ==  1) direction = Direction::SW;
  if (stepDx == -1 && stepDy ==  0) direction = Direction::W;
  if (stepDx == -1 && stepDy == -1) direction = Direction::NW;
  return direction;
}
} // namespace direction

#endif /* DIRECTION_HPP */
