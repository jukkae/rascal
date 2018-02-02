#ifndef LOS_HPP
#define LOS_HPP
#include "constants.hpp"
#include "point.hpp"

class Map;
class Actor;
namespace los {
bool is_visible(Point a, Point b, Map* map, std::vector<Actor*> actors, int radius = constants::DEFAULT_ENEMY_FOV_RADIUS);
float diagonal_distance(Point a, Point b); //FIXME collect this and all other distance functions
Point round_point(Point p);
float lerp(int a, int b, float t);
Point lerp_point(Point a, Point b, float t);
std::vector<Point> get_line(Point a, Point b);
}; // namespace los
#endif /* LOS_HPP */
