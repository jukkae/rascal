#include "fov.hpp"
#include "map.hpp"
#include "vec.hpp"

void fov::computeFov(Map* map, int x, int y, int radius, FovType fovType) {
	//Low-hanging fruit optimization:
	//Only loop through possible values
	for(int i = 0; i < map->width; ++i) {
		for(int j = 0; j < map->height; ++j) {
			map->tiles[i + map->width*j].inFov = false;
		}
	}
	for(int octant = 0; octant < 8; octant++) {
		computeFovForOctant(map, x, y, octant, radius, fovType);
	}
}

void fov::computeFovForOctant(Map* map, int x, int y, int octant, int radius, FovType fovType) {
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
				map->tiles[xPos + map->width*yPos].inFov = false;
			}
			else {
				Shadow projection = Shadow::projectTile(row, col);
				bool visible = !shadowLine.isInShadow(projection);
				map->tiles[xPos + map->width*yPos].inFov = visible;
				if(visible) map->tiles[xPos + map->width*yPos].explored = true; // *maybe* extract function

				if(visible && map->isWall(xPos, yPos) /*TODO check for doors*/) {
					shadowLine.addShadow(projection);
					fullShadow = shadowLine.isFullShadow();
				}
			}
		}
	}
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

