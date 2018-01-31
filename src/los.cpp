#include "los.hpp"

#include "map.hpp"
#include "actor.hpp"
#include "status_effect.hpp" //gorram

bool los::is_visible(Point a, Point b, Map* map, std::vector<Actor*> actors, int radius) {
	if(std::sqrt((b.x-a.x)*(b.x-a.x) + (b.y-a.y)*(b.y-a.y)) > radius) return false;
	auto line = los::get_line(a, b);
	for(auto a : line) {
		if (map->isWall(a.x, a.y)) return false;
		for(auto b : actors) if (a.x == b->x && a.y == b->y) {
			if (b->blocksLight) return false;
		}
	}
	return true;
}

float los::diagonal_distance(Point a, Point b) {
	float dx = b.x - a.x;
	float dy = b.y - a.y;
	return std::max(std::abs(dx), std::abs(dy));
}

Point los::round_point(Point p) {
	return Point(std::round(p.x), std::round(p.y));
}

float los::lerp(int a, int b, float t) {
	return a + t*(b-a);
}

Point los::lerp_point(Point a, Point b, float t) {
	return Point(lerp(a.x, b.x, t), lerp(a.y, b.y, t));
}

std::vector<Point> los::get_line(Point a, Point b) {
	std::vector<Point> line;
	int n = los::diagonal_distance(a, b) + 1;
	for(int i = 0; i < n; ++i) {
		float t = (n == 0 ? 0.0 : (float)i / (float)n);
        line.push_back(round_point(lerp_point(a, b, t)));
	}
	if(line.back().x != b.x && line.back().y != b.y) line.push_back(b);
	return line;
}
