#ifndef DICE_HPP
#define DICE_HPP

#include <random>

static std::default_random_engine generator;
static std::uniform_int_distribution<int> d2_dist  (1, 2);
static std::uniform_int_distribution<int> d3_dist  (1, 3);
static std::uniform_int_distribution<int> d4_dist  (1, 4);
static std::uniform_int_distribution<int> d6_dist  (1, 6);
static std::uniform_int_distribution<int> d8_dist  (1, 8);
static std::uniform_int_distribution<int> d10_dist (1, 10);
static std::uniform_int_distribution<int> d12_dist (1, 12);
static std::uniform_int_distribution<int> d20_dist (1, 20);
static std::uniform_int_distribution<int> d100_dist(1, 100);
static auto d2   = []() { return d2_dist  (generator); };
static auto d3   = []() { return d3_dist  (generator); };
static auto d4   = []() { return d4_dist  (generator); };
static auto d6   = []() { return d6_dist  (generator); };
static auto d8   = []() { return d8_dist  (generator); };
static auto d10  = []() { return d10_dist (generator); };
static auto d12  = []() { return d12_dist (generator); };
static auto d20  = []() { return d20_dist (generator); };
static auto d100 = []() { return d100_dist(generator); };

#endif /* DICE_HPP */
