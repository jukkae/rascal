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
auto d2   = std::bind( d2_dist,   generator );
auto d4   = std::bind( d4_dist,   generator );
auto d6   = std::bind( d6_dist,   generator );
auto d8   = std::bind( d8_dist,   generator );
auto d10  = std::bind( d10_dist,  generator );
auto d12  = std::bind( d12_dist,  generator );
auto d20  = std::bind( d20_dist,  generator );
auto d100 = std::bind( d100_dist, generator );
