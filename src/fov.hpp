#ifndef FOV_HPP
#define FOV_HPP

#include "constants.hpp"

class Map;
template<class T>
struct Vec;


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
	void computeFov(Map* map, /*std::vector<Actor*> actors,*/ int x, int y, int radius = constants::DEFAULT_FOV_RADIUS, FovType fovType = FovType::CIRCLE);
	void computeFovForOctant(Map* map, int x, int y, int octant, int radius = constants::DEFAULT_FOV_RADIUS, FovType fovType = FovType::CIRCLE);
	Vec<int> transformOctant(int row, int col, int octant);
}; // namespace fov
#endif /* FOV_HPP */
