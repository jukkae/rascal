#include <random>

std::default_random_engine generator;
std::uniform_int_distribution<int> d2_dist  (1, 2);
std::uniform_int_distribution<int> d4_dist  (1, 4);
std::uniform_int_distribution<int> d6_dist  (1, 6);
std::uniform_int_distribution<int> d8_dist  (1, 8);
std::uniform_int_distribution<int> d10_dist (1, 10);
std::uniform_int_distribution<int> d12_dist (1, 12);
std::uniform_int_distribution<int> d20_dist (1, 20);
std::uniform_int_distribution<int> d100_dist(1, 100);
auto d2   = []() { return d2_dist  (generator); };
auto d4   = []() { return d4_dist  (generator); };
auto d6   = []() { return d6_dist  (generator); };
auto d8   = []() { return d8_dist  (generator); };
auto d10  = []() { return d10_dist (generator); };
auto d12  = []() { return d12_dist (generator); };
auto d20  = []() { return d20_dist (generator); };
auto d100 = []() { return d100_dist(generator); };
