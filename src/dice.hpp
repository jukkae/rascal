#ifndef DICE_HPP
#define DICE_HPP

#include <random>

namespace dice { // FIXME bad, but better than what it was
  extern std::default_random_engine& gen;
}

int d2();
int d3();
int d4();
int d5();
int d6();
int d8();
int d10();
int d12();
int d20();
int d100();
int d1000();

#endif /* DICE_HPP */
