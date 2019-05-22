#include "pathfinding.hpp"
#include "map.hpp"
#include "point.hpp"

#include <unordered_map>

std::vector<Point> pathfinding::findPath(Map map, Point from, Point to) {

	Mat2d<Tile> tiles = map.tiles;
	std::vector<Point> path;
	if(to.x < 0 || to.x >= tiles.w || to.y < 0 || to.y >= tiles.h ||
		 !tiles.at(to.x, to.y).walkable) return path; // TODO figure out more robust handling

	PriorityQueue<Point, float> frontier;
	frontier.put(from, 0.0f);

	std::unordered_map<Point, Point, PointHasher> came_from;
	came_from[from] = from;

	std::unordered_map<Point, float, PointHasher> cost_thus_far;
	cost_thus_far[from] = 0.0f;

	while(!frontier.empty()) {
		Point current = frontier.get();

		if (current == to) {
			break;
		}
		for(int i = -1; i <= 1; ++i) {
			for(int j = -1; j <= 1; ++j) {
				if(i == 0 && j == 0) continue;
				Point next = Point(current.x + i, current.y + j);
				if(next.x < 0 || next.y < 0 || next.x >= tiles.w || next.y >= tiles.h) continue;
				float nextDeltaCost = (tiles.at(current.x, current.y).movementCost +
															 tiles.at(next.x, next.y).movementCost) / 2.0f;
				if((i != 0) && (j != 0)) nextDeltaCost *= sqrtf(2.0);
				float nextCost = cost_thus_far[current] + nextDeltaCost;
				if(cost_thus_far.find(next) == cost_thus_far.end() ||
					 nextCost < cost_thus_far[next]) {
			 		if(tiles.at(next.x, next.y).walkable) {
						cost_thus_far[next] = nextCost;
						float heuristic = sqrtf(pow(current.x - next.x, 2) +
						                        pow(current.y - next.y, 2));
						float priority = nextCost + heuristic;
						came_from[next] = current;
						frontier.put(next, priority);
					}
				}
			}
		}
	}

	Point current = to;

	while(!(current == from)) {
		path.push_back(current);
		current = came_from[current];
	}
	path.push_back(from);
	std::reverse(path.begin(), path.end());

	return path;
}
