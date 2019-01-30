#include "fov.hpp"

#include "actor.hpp"
#include "status_effect.hpp" //FIXME wtf do i need this for
#include "map.hpp"
#include "vec.hpp"

std::set<Point> fov::computeEnemyFov(Map* map, int x, int y, Direction direction, int radius, FovType fovType, std::vector<Actor*> actors) {
	std::set<Point> fovTiles { };
	std::vector<int> octants;
	switch(direction) {
		case Direction::NONE: radius /= 4; octants = {0, 1, 2, 3, 4, 5, 6, 7}; break;
		case Direction::N:  octants = {0, 7}; break;
		case Direction::NE: octants = {0, 1}; break;
		case Direction::E:  octants = {1, 2}; break;
		case Direction::SE: octants = {2, 3}; break;
		case Direction::S:  octants = {3, 4}; break;
		case Direction::SW: octants = {4, 5}; break;
		case Direction::W:  octants = {5, 6}; break;
		case Direction::NW: octants = {6, 7}; break;
	}
	for(auto n : octants) {
		auto octantTiles = computeEnemyFovForOctant(map, x, y, n, radius, fovType, actors);
		fovTiles.insert(octantTiles.begin(), octantTiles.end());
	}
	return fovTiles;
}

std::set<Point> fov::computeEnemyFovForOctant(Map* map, int x, int y, int octant, int radius, FovType fovType, std::vector<Actor*> actors) {
	std::set<Point> fovTiles { };
	ShadowLine shadowLine;
	bool fullShadow = false;
	for(int row = 0; row < radius; row++) {
		for(int col = 0; col <= row; col++) {
			if(fovType == FovType::CIRCLE) {
				if(sqrt(row*row + col*col) > radius) break;
			} // else if fovType == FovType::SQUARE
			int xPos = x + transformOctant(row, col, octant).x;
			int yPos = y + transformOctant(row, col, octant).y;

			if(xPos < 0 || xPos >= map->width || yPos < 0 || yPos >= map->height) break;

			if(fullShadow) {
				map->tiles(xPos, yPos).inEnemyFov = false;
			}
			else {
				Shadow projection = Shadow::projectTile(row, col);
				bool visible = !shadowLine.isInShadow(projection);
				map->tiles(xPos, yPos).inEnemyFov = visible;
				if(visible) fovTiles.emplace(Point{xPos, yPos});
				//if(visible) map->tiles(xPos, yPos).explored = true; // *maybe* extract function

				if(visible && map->isWall(xPos, yPos) /*TODO check for doors*/) {
					shadowLine.addShadow(projection);
					fullShadow = shadowLine.isFullShadow();
				}
				if(visible && actors.size() > 0) {
					for(auto& a : actors) {
						if(a->blocksLight && a->x == xPos && a->y == yPos) {
							shadowLine.addShadow(projection);
							fullShadow = shadowLine.isFullShadow();
						}
					}
				}
			}
		}
	}
	return fovTiles;
}

std::set<Point> fov::computeFov(Map* map, int x, int y, int radius, FovType fovType, std::vector<Actor*> actors) {
	std::set<Point> fovTiles { };
	//Low-hanging fruit optimization:
	//Only loop through possible values
	for(int i = 0; i < map->width; ++i) {
		for(int j = 0; j < map->height; ++j) {
			map->tiles(i, j).inFov = false;
		}
	}
	for(int octant = 0; octant < 8; octant++) {
		auto octantTiles = computeFovForOctant(map, x, y, octant, radius, fovType, actors);
		fovTiles.insert(octantTiles.begin(), octantTiles.end());
	}
	return fovTiles;
}

std::set<Point> fov::computeFovForOctant(Map* map, int x, int y, int octant, int radius, FovType fovType, std::vector<Actor*> actors) {
	std::set<Point> fovTiles { };
	ShadowLine shadowLine;
	bool fullShadow = false;
	for(int row = 0; row < radius; row++) {
		for(int col = 0; col <= row; col++) {
			if(fovType == FovType::CIRCLE) {
				if(sqrt(row*row + col*col) > radius) break;
			} // else if fovType == FovType::SQUARE
			int xPos = x + transformOctant(row, col, octant).x;
			int yPos = y + transformOctant(row, col, octant).y;

			if(xPos < 0 || xPos >= map->width || yPos < 0 || yPos >= map->height) break;

			if(fullShadow) {
				map->tiles(xPos, yPos).inFov = false;
			}
			else {
				Shadow projection = Shadow::projectTile(row, col);
				bool visible = !shadowLine.isInShadow(projection);

				map->tiles(xPos, yPos).inFov = visible;
				if(visible) fovTiles.emplace(Point{xPos, yPos});

				if(visible) map->tiles(xPos, yPos).explored = true; // *maybe* extract function

				if(visible && map->isWall(xPos, yPos) /*TODO check for doors*/) {
					shadowLine.addShadow(projection);
					fullShadow = shadowLine.isFullShadow();
				}
				if(visible && actors.size() > 0) {
					for(auto& a : actors) {
						if(a->blocksLight && a->x == xPos && a->y == yPos) {
							shadowLine.addShadow(projection);
							fullShadow = shadowLine.isFullShadow();
						}
					}
				}
			}
		}
	}
	return fovTiles;
}

Vec<int> fov::transformOctant(int row, int col, int octant) {
	switch(octant) {
		case 0: return Vec<int>( col, -row);
		case 1: return Vec<int>( row, -col);
		case 2: return Vec<int>( row,  col);
		case 3: return Vec<int>( col,  row);
		case 4: return Vec<int>(-col,  row);
		case 5: return Vec<int>(-row,  col);
		case 6: return Vec<int>(-row, -col);
		case 7: return Vec<int>(-col, -row);
	}
	return Vec<int> (0, 0); // should throw
}

Shadow Shadow::projectTile(int row, int column) {
	float topLeft = column / (row + 2.0);
	float bottomRight = (column + 1.0) / (row + 1.0);
	return Shadow(topLeft, bottomRight);
}

// Check if other is fully covered by this
bool Shadow::contains(Shadow other) {
	return start <= other.start && end >= other.end;
}

// Check if projection of tile is in shadow
bool ShadowLine::isInShadow(Shadow projection) {
	for(auto shadow : shadows) {
		if(shadow.contains(projection)) return true;
	}
	return false;
}

void ShadowLine::addShadow(Shadow shadow) {
	int index = 0;
	for(; index < shadows.size(); index++) {
		if(shadows[index].start >= shadow.start) break;
	}

	Shadow* overlappingPrevious = nullptr;
	if(index > 0 && shadows[index - 1].end > shadow.start) {
		overlappingPrevious = &shadows[index - 1];
	}

	Shadow* overlappingNext = nullptr;
	if(index < shadows.size() && shadows[index].start < shadow.end) {
		overlappingNext = &shadows[index];
	}

	if(overlappingNext) {
		if(overlappingPrevious) {
			overlappingPrevious->end = overlappingNext->end;
			shadows.erase(shadows.begin() + index);
		}
		else {
			overlappingNext->start = shadow.start;
		}
	}
	else {
		if(overlappingPrevious) {
			overlappingPrevious->end = shadow.end;
		}
		else {
			shadows.insert(shadows.begin() + index, shadow);
		}
	}
}
