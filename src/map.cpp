#include <algorithm>
#include <cmath>
#include <limits>
#include "actor.hpp"
#include "attacker.hpp"
#include "colors.hpp"
#include "constants.hpp"
#include "destructible.hpp"
#include "dice.hpp"
#include "gameplay_state.hpp"
#include "map.hpp"
#include "pickable.hpp"
#include "point.hpp"
#include "rect.hpp"

#include <SFML/Graphics/Color.hpp>

Map::Map() : Map(constants::DEFAULT_MAP_WIDTH, constants::DEFAULT_MAP_HEIGHT) {}

Map::Map(int width, int height) : width(width), height(height) {
}

Map::~Map() {
}

void Map::init(bool initActors) {
	for(int i = 0; i < width*height; ++i) {
		tiles.push_back(Tile());
	}

	generateMap();

	addItems();
	addMonsters();
}

void Map::generateMap() {
	std::vector<Rect> rooms;
	std::vector<Rect> areas = breakRooms(Rect(0, 0, (width - 1), (height - 1))); // TODO dimensions

	// initialize
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			tiles.at(x + y*width).walkable = true;
		}
	}

	// generate outer walls
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			if(x == 0 || x == width - 1 || y == 0 || y == height - 1) tiles.at(x + y*width).walkable = false;
		}
	}

	for(auto a : areas) {
		for(int x = a.x0(); x < a.x1(); ++x) {
			for(int y = a.y0(); y < a.y1(); ++y) {
				if(x == a.x0() || x == a.x1() || y == a.y0() || y == a.y1()) {
					tiles.at(x + y*width).walkable = false;
				}
				if(x == a.x0() + 1 || x == a.x1() - 1 || y == a.y0() + 1 || y == a.y1() - 1) {
					tiles.at(x + y*width).walkable = true;
				}
			}
		}
	}

	/*for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			if(x % 3 == 0 && y % 3 == 0) tiles.at(x + y*width).walkable = false;
		}
	}*/
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

Actor* Map::makeMonster(int x, int y) {
	int r = d100();
	if(r < 70) {
		Actor* punk = new Actor(x, y, 'h', "punk", colors::desaturatedGreen, 1);
		punk->destructible = std::make_unique<MonsterDestructible>(10, 0, 50, "dead punk", 13);
		punk->attacker = std::make_unique<Attacker>(3);
		punk->ai = std::make_unique<MonsterAi>();
		return punk;
	} else if (r < 80) {
		Actor* fighter = new Actor(x, y, 'H', "fighter", colors::darkGreen, 1);
		fighter->destructible = std::make_unique<MonsterDestructible>(16, 1, 100, "fighter carcass", 15);
		fighter->attacker = std::make_unique<Attacker>(4);
		fighter->ai = std::make_unique<MonsterAi>();
		return fighter;
	}
	else {
		Actor* boxer = new Actor(x, y, 'H', "boxer", colors::darkerGreen, 1);
		boxer->destructible = std::make_unique<MonsterDestructible>(4, 1, 70, "boxer carcass", 16);
		boxer->attacker = std::make_unique<Attacker>(4);
		boxer->ai = std::make_unique<MonsterAi>();
		return boxer;
	}
}

void Map::addMonsters() {
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			int r = d100();
			if (canWalk(x, y) && r == 1) {
				addMonster(x, y);
			}
		}
	}
}

void Map::addItems() {
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			int r = d100();
			if (canWalk(x, y) && r <= 2) {
				addItem(x, y);
			}
		}
	}
}

void Map::addMonster(int x, int y) {
	state->addActor(makeMonster(x, y));
}

// TODO colors!
Actor* Map::makeItem(int x, int y) {
	int r = d100();
	if(r < 60) {
		Actor* stimpak = new Actor(x, y, '!', "stimpak", sf::Color(128, 0, 128));
		stimpak->blocks = false;
		stimpak->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::WEARER, 0), std::make_unique<HealthEffect>(4));
		return stimpak;
	} else if(r < 70) {
		Actor* blasterBoltDevice = new Actor(x, y, '?', "blaster bolt device", sf::Color(128, 128, 0));
		blasterBoltDevice->blocks = false;
		blasterBoltDevice->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::CLOSEST_MONSTER, 5), std::make_unique<HealthEffect>(-20, "The %s is hit by a blast!\n The damage is %g hit points."));
		return blasterBoltDevice;
	} else if(r < 80) {
		Actor* fragGrenade = new Actor(x, y, '?', "fragmentation grenade", sf::Color(128, 255, 128));
		fragGrenade->blocks = false;
		fragGrenade->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::SELECTED_RANGE, 3), std::make_unique<HealthEffect>(-12, "The grenade explodes, hurting the %s for %g hit points!"));
		return fragGrenade;
	} else if(r < 90) {
		Actor* confusor = new Actor(x, y, '?', "confusor", sf::Color(128, 128, 255));
		confusor->blocks = false;
		confusor->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::SELECTED_MONSTER, 5), std::make_unique<AiChangeEffect>(std::make_unique<ConfusedMonsterAi>(10), "The eyes of the %s look vacant!"));
		return confusor;
	} else {
		Actor* teslaCoil = new Actor(x, y, '#', "tesla coil", sf::Color(128, 128, 255));
		teslaCoil->blocks = false;
		teslaCoil->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::WEARER_RANGE, 5), std::make_unique<HealthEffect>(-6, "The tesla coil sputters, emitting raw\n electricity, hurting %s for %g hp!"));
		return teslaCoil;
	}
}

void Map::addItem(int x, int y) {
	state->addActor(makeItem(x, y));
}

bool Map::isWall(int x, int y) const {
	return !tiles.at(x + y*width).walkable; // can't use this while initialising monsters
	//return !map->isWalkable(x, y);
}

bool Map::canWalk(int x, int y) const {
	if(isWall(x, y)) return false;
	if(!state->canWalk(x, y)) return false;
	return true;
}

bool Map::isExplored(int x, int y) const {
	return tiles[x + y*width].explored;
}

bool Map::isInFov(int x, int y) const {
	if(x < 0 || x >= width || y < 0 || y >= height) return false;
	if(tiles[x+y*width].inFov) {
		return true;
	}
	return false;
	//return isInFovNew(x, y);
}

void Map::computeFov(FovType fovType) {
	for(int x = 0; x < width; x++) {
		for(int y = 0; y < height; y++) {
			tiles[x + width*y].inFov = false;
		}
	}
	int playerX = state->getPlayer()->x;
	int playerY = state->getPlayer()->y;
	for(int octant = 0; octant < 8; octant++) {
		computeFovForOctant(playerX, playerY, octant, fovType);
	}
}

void Map::computeFovForOctant(int x, int y, int octant, FovType fovType) {
	ShadowLine shadowLine;
	bool fullShadow = false;
	for(int row = 0; row < constants::DEFAULT_FOV_RADIUS; row++) {
		for(int col = 0; col <= row; col++) {
			if(fovType == FovType::CIRCLE) {
				if(sqrt(row*row + col*col) > constants::DEFAULT_FOV_RADIUS) break;
			} // else if fovType == FovType::SQUARE
			int xPos = x + transformOctant(row, col, octant).x;
			int yPos = y + transformOctant(row, col, octant).y;

			if(xPos < 0 || xPos >= width || yPos < 0 || yPos >= height) break;

			if(fullShadow) {
				tiles[xPos + width*yPos].inFov = false;
			}
			else {
				Shadow projection = Shadow::projectTile(row, col);
				bool visible = !shadowLine.isInShadow(projection);
				tiles[xPos + width*yPos].inFov = visible;
				if(visible) tiles[xPos + width*yPos].explored = true; // *maybe* extract function

				if(visible && isWall(xPos, yPos)) {
					shadowLine.addShadow(projection);
					fullShadow = shadowLine.isFullShadow();
				}
			}
		}
	}
}

Vec<int> Map::transformOctant(int row, int col, int octant) {
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
