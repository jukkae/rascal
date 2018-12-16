#include "dice.hpp"
#include <chrono>

// could use std::random_device{}() instead of time, but should check for entropy
unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine generator(seed);

std::default_random_engine& dice::gen = generator;

std::uniform_int_distribution<int> d2_dist   (1, 2);
std::uniform_int_distribution<int> d3_dist   (1, 3);
std::uniform_int_distribution<int> d4_dist   (1, 4);
std::uniform_int_distribution<int> d5_dist   (1, 5);
std::uniform_int_distribution<int> d6_dist   (1, 6);
std::uniform_int_distribution<int> d8_dist   (1, 8);
std::uniform_int_distribution<int> d10_dist  (1, 10);
std::uniform_int_distribution<int> d12_dist  (1, 12);
std::uniform_int_distribution<int> d20_dist  (1, 20);
std::uniform_int_distribution<int> d100_dist (1, 100);
std::uniform_int_distribution<int> d1000_dist(1, 1000);

int d2()    { return d2_dist   (generator); }
int d3()    { return d3_dist   (generator); }
int d4()    { return d4_dist   (generator); }
int d5()    { return d5_dist   (generator); }
int d6()    { return d6_dist   (generator); }
int d8()    { return d8_dist   (generator); }
int d10()   { return d10_dist  (generator); }
int d12()   { return d12_dist  (generator); }
int d20()   { return d20_dist  (generator); }
int d100()  { return d100_dist (generator); }
int d1000() { return d1000_dist(generator); }
