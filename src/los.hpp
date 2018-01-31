#ifndef LOS_HPP
#define LOS_HPP
#include "point.hpp"

namespace los {
bool is_visible(Point a, Point b);
float diagonal_distance(Point a, Point b); //FIXME collect this and all other distance functions
Point round_point(Point p);
float lerp(int a, int b, float t);
Point lerp_point(Point a, Point b, float t);
std::vector<Point> get_line(Point a, Point b);
}; // namespace los
#endif /* LOS_HPP */
