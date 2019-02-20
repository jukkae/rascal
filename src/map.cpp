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

Map::Map() :
Map(constants::DEFAULT_MAP_WIDTH,
	  constants::DEFAULT_MAP_HEIGHT,
		MapType::BUILDING) { }

Map::Map(int width, int height, MapType mapType) :
width(width), height(height) {
	tiles = Mat2d<Tile>(width, height);
	for(int i = 0; i < width; ++i) {
		for(int j = 0; j < height; ++j) {
			tiles(i, j) = Tile();
		}
	}
	generateMap(mapType);
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
	// DEBUG PRINT
	std::cout << "MAP TILES:\n";
	int x = 0;
	int y = 0;
	for(auto& tile : tiles) {
		++x;
		if(!tile.walkable) std::cout << "#";
		else {
			if(getRooms(Point {x, y}).size() == 0) {
				std::cout << ".";
			} else if(getRooms(Point {x, y}).at(0).roomType == RoomType::COMMAND_CENTER) {
				std::cout << "c";
			} else {
				std::cout << ".";
			}
		}
		if(x >= width) {
			std::cout << "\n";
			x = 0;
			++y;
		}
	}
	std::cout << "END MAP TILES\n";
	std::cout << "MAP ROOMS:\n";
	for(auto& room : rooms) {
		std::cout << "room "
		<< (room.roomType == RoomType::COMMAND_CENTER ? "(command center)" : "(normal)")
		<< ": "
		<< "(" << room.coordinates.x0() << ", " << room.coordinates.y0() << "), "
		<< "(" << room.coordinates.x1() << ", " << room.coordinates.y1() << ")\n";
		std::cout << "  neighbours: " << room.neighbours.size() << "\n";
	}
	std::cout << "END MAP ROOMS\n";
}

std::vector<Room> Map::getRooms(Point location) {
	std::vector<Room> r;
	for(auto room : rooms) {
		if(   location.x >= room.x0()
			 && location.x <= room.x1()
		   && location.y >= room.y0()
		   && location.y <= room.y1()) {
			r.push_back(room);
		}
	}
	return r;
}

void Map::generateBuildingMap() {
	rooms = breakRooms(Rect(0, 0, (width - 1), (height - 1)));
	rooms = connectRooms(rooms);

	// initialize whole map to walkable
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			if(true) {
				tiles(x, y).walkable = true;
			}
		}
	}

	// draw walls
	for(auto& a : rooms) {
		for(int x = a.x0(); x <= a.x1(); ++x) {
			for(int y = a.y0(); y <= a.y1(); ++y) {
				if(x == a.x0() || x == a.x1() || y == a.y0() || y == a.y1()) {
					tiles(x, y).walkable = false;
				}
				// if(x == a.x0() + 1 || x == a.x1() - 1 || y == a.y0() + 1 || y == a.y1() - 1) {
				// 	//tiles(x, y).walkable = true;
				// }
			}
		}
	}

	// open doors
	for(auto& a : rooms) {
		int centerX = floor((a.x1() + a.x0()) / 2);
		int centerY = floor((a.y1() + a.y0()) / 2);
		// std::cout << "Area: "
		// << "(" << a.x0() << ", " << a.y0() << "), "
		// << "(" << a.x1() << ", " << a.y1() << ")\n";
		// std::cout << "Centers: "
		// << "(" << centerX << ", " << centerY << ")\n";

		tiles(a.x0(), centerY).walkable = true;
		tiles(a.x1(), centerY).walkable = true;
		tiles(centerX, a.y0()).walkable = true;
		tiles(centerX, a.y1()).walkable = true;
	}

	// close map boundaries
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
			anim.colors.push_back(colors::get("darkestBlue"));
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

std::vector<Room> Map::breakRooms(Rect area, BreakDirection direction) {
	int minDim = 20;
	std::vector<Room> areas;

	if(area.width() < minDim || area.height() < minDim) {
		if(d10() == 10) {
			areas.push_back({area, RoomType::COMMAND_CENTER, RoomDecor::NONE});
		} else areas.push_back({area, RoomType::NORMAL, RoomDecor::NONE});
		return areas;
	}
	else {
		Rect area1 = area;
		Rect area2 = area;
		if(direction == BreakDirection::HORIZONTAL) {
			// 3 = minimum width for a corridor
			int xBreak = randomInRange(3, area.width()-3);
			// if xBreak - 0 were xBreak - 1, this would cause areas to NOT overlap
			area1 = Rect(area.x0(),          area.y0(), area.x0() + xBreak - 0, area.y1());
			area2 = Rect(area.x0() + xBreak, area.y0(), area.x1(),              area.y1());
		} else {
			int yBreak = randomInRange(3, area.height()-3);
			area1 = Rect(area.x0(), area.y0(),          area.x1(), area.y0() + yBreak - 0);
			area2 = Rect(area.x0(), area.y0() + yBreak, area.x1(), area.y1()             );
		}

		BreakDirection nextDir = (direction == BreakDirection::HORIZONTAL) ? BreakDirection::VERTICAL : BreakDirection::HORIZONTAL;

		std::vector<Room> areas1 = breakRooms(area1, nextDir);
		areas.insert(areas.end(), areas1.begin(), areas1.end());
		std::vector<Room> areas2 = breakRooms(area2, nextDir);
		areas.insert(areas.end(), areas2.begin(), areas2.end());

		return areas;
	}
}

std::vector<Room> Map::connectRooms(std::vector<Room> rooms) {
	std::vector<Room> ret = rooms;
	for(auto& room : ret) {
		for(auto& other : ret) {
			if(room == other) continue;
			// check if overlap
			if(room.x0() > other.x1() || other.x0() > room.x1()) continue; // no overlap
			if(room.y0() > other.y1() || other.y0() > room.y1()) continue; // no overlap
			// overlap!
			// std::cout << "overlap: "
			// << "(" << room.x0() << ", " << room.y0() << "), (" << room.x1() << ", " << room.y1() << ")"
			// << " with "
			// << "(" << other.x0() << ", " << other.y0() << "), (" << other.x1() << ", " << other.y1() << ")"
			// << "\n";
			room.neighbours.emplace_back(&other);
		}
	}
	return ret;
}

bool Map::isWall(int x, int y) const {
	return !tiles(x, y).walkable;
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
