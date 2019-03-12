#ifndef FOV_HPP
#define FOV_HPP
#include <set>
#include <vector>

#include "constants.hpp"
#include "direction.hpp"

class Actor;
class Map;
template<class T>
struct Vec;
#include "point.hpp"


enum class FovType { SQUARE, CIRCLE };

class Shadow {
public:
	Shadow(float start, float end) : start(start), end(end) {;}
	float start;
	float end;

	static Shadow projectTile(int row, int column);
	bool contains(Shadow other);

};

class ShadowLine {
public:
	std::vector<Shadow> shadows;

	bool isInShadow(Shadow projection);
	bool isFullShadow() { return shadows.size() == 1 && shadows[0].start == 0 && shadows[0].end == 1; }
	void addShadow(Shadow shadow);
};

namespace fov {
	std::set<Point> computeEnemyFov(Map* map, int x, int y, Direction direction, int radius = constants::DEFAULT_FOV_RADIUS, FovType fovType = FovType::CIRCLE, std::vector<Actor*> actors = {});
	std::set<Point> computeEnemyFovForOctant(Map* map, int x, int y, int octant, int radius = constants::DEFAULT_FOV_RADIUS, FovType fovType = FovType::CIRCLE, std::vector<Actor*> actors = {});
	std::set<Point> computeFov(Map* map, int x, int y, int radius = constants::DEFAULT_FOV_RADIUS, FovType fovType = FovType::CIRCLE, std::vector<Actor*> actors = {});
	std::set<Point> computeFovForOctant(Map* map, int x, int y, int octant, int radius = constants::DEFAULT_FOV_RADIUS, FovType fovType = FovType::CIRCLE, std::vector<Actor*> actors = {});
	Vec<int> transformOctant(int row, int col, int octant);
}; // namespace fov
#endif /* FOV_HPP */
