#include <algorithm>
#include <cmath>
#include <limits>
#include "actor.hpp"
#include "attacker.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "destructible.hpp"
#include "dice.hpp"
#include "effect.hpp"
#include "map.hpp"
#include "map_utils.hpp"
#include "pickable.hpp"
#include "point.hpp"
#include "rect.hpp"
#include "status_effect.hpp"
#include "vec.hpp"
#include "world.hpp"

#include <SFML/Graphics/Color.hpp>

Map::Map() : Map(constants::DEFAULT_MAP_WIDTH, constants::DEFAULT_MAP_HEIGHT) {}

Map::Map(int width, int height) : width(width), height(height) {
	tiles = Mat2d<Tile>(width, height);
	for(int i = 0; i < width; ++i) {
		for(int j = 0; j < height; ++j) {
			tiles(i, j) = Tile();
		}
	}
}

void Map::generateMap(MapType mapType) {
	switch(mapType) {
		case MapType::BUILDING:
			generateBuildingMap();
			break;
		case MapType::PILLARS:
			generatePillarsMap();
			break;
		case MapType::WATER:
			generateWaterMap();
			break;
		default:
			break;
	}
}

void Map::generateBuildingMap() {
	std::vector<Rect> rooms;
	std::vector<Rect> areas = breakRooms(Rect(0, 0, (width - 1), (height - 1)));

	for(auto a : areas) {
		for(int x = a.x0(); x < a.x1(); ++x) {
			for(int y = a.y0(); y < a.y1(); ++y) {
				if(x == a.x0() || x == a.x1() || y == a.y0() || y == a.y1()) {
					tiles(x, y).walkable = false;
				}
				if(x == a.x0() + 1 || x == a.x1() - 1 || y == a.y0() + 1 || y == a.y1() - 1) {
					tiles(x, y).walkable = true;
				}
			}
		}
	}

	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			if(x == 0 || x == width - 1 || y == 0 || y == height - 1) {
				tiles(x, y).walkable = false;
			}
		}
	}
}

void Map::generatePillarsMap() {
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			if(x % 3 == 0 && y % 3 == 0) tiles(x, y).walkable = false;
		}
	}
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			if(x == 0 || x == width - 1 || y == 0 || y == height - 1) {
				tiles(x, y).walkable = false;
			}
		}
	}
}

void Map::generateWaterMap() {
	hasAnimations = true;
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			tiles(x, y) = Tile();
			tiles(x, y).terrain = Terrain::WATER;
			Animation anim;
			anim.chars = std::vector<char>(); // empty on purpose
			anim.colors = std::vector<sf::Color>();
			anim.colors.push_back(colors::darkestBlue);
			anim.charFreq = 1;
			anim.colFreq = 10;
			anim.phase = 0.0f;
			tiles(x, y).animation = anim;
		}
	}
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			if(x % 3 == 0 && y % 3 == 0) {
				tiles(x, y).walkable = false;
				tiles(x, y).animation = std::experimental::nullopt;
			}
		}
	}
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			if(x == 0 || x == width - 1 || y == 0 || y == height - 1) {
				tiles(x, y).walkable = false;
				tiles(x, y).animation = std::experimental::nullopt;
			}
		}
	}
}

std::vector<Rect> Map::breakRooms(Rect area, BreakDirection direction) {
	int minDim = 30;
	std::vector<Rect> areas;
	
	if(area.width() < minDim || area.height() < minDim) {
		areas.push_back(area);
		return areas;
	}
	else {
		Rect area1 = area;
		Rect area2 = area;
		if(direction == BreakDirection::HORIZONTAL) {
			int d = d20();
			int xBreak = d * area.width() / 20;
			//int xBreak = area.width() / 2;
			area1 = Rect(area.x0(),          area.y0(), area.x0() + xBreak - 1, area.y1());
			area2 = Rect(area.x0() + xBreak, area.y0(), area.x1(),              area.y1());
		} else {
			int d = d20();
			int yBreak = d * area.height() / 20;
			//int yBreak = area.height() / 2;
			area1 = Rect(area.x0(), area.y0(),          area.x1(), area.y0() + yBreak - 1);
			area2 = Rect(area.x0(), area.y0() + yBreak, area.x1(), area.y1()             );
		}

		BreakDirection nextDir = (direction == BreakDirection::HORIZONTAL) ? BreakDirection::VERTICAL : BreakDirection::HORIZONTAL;

		std::vector<Rect> areas1 = breakRooms(area1, nextDir);
		areas.insert(areas.end(), areas1.begin(), areas1.end());
		std::vector<Rect> areas2 = breakRooms(area2, nextDir);
		areas.insert(areas.end(), areas2.begin(), areas2.end());

		return areas;
	}
}

bool Map::isWall(int x, int y) const {
	return !tiles(x, y).walkable;
	//return !map->isWalkable(x, y); // can't use this while creating monsters
}

bool Map::canWalk(int x, int y) const {
	if(isWall(x, y)) return false;
	if(!world->canWalk(x, y)) return false;
	return true;
}

bool Map::isExplored(int x, int y) const {
	if(x < 0 || x >= width || y < 0 || y >= height) return false; // I mean, if it's not on map, it can't be explored, right?
	return tiles(x, y).explored;
}

bool Map::isInFov(int x, int y) const {
	if(x < 0 || x >= width || y < 0 || y >= height) return false;
	if(tiles(x, y).inFov) {
		return true;
	}
	return false;
}

