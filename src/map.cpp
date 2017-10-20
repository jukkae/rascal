#include <algorithm>
#include <cmath>
#include <limits>
#include "actor.hpp"
#include "attacker.hpp"
#include "constants.hpp"
#include "destructible.hpp"
#include "dice.hpp"
#include "gameplay_state.hpp"
#include "map.hpp"
#include "pickable.hpp"

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;
static const int MAX_ROOM_MONSTERS = 3;
static const int MAX_ROOM_ITEMS = 2;

Map::Map() : Map(constants::DEFAULT_MAP_WIDTH, constants::DEFAULT_MAP_HEIGHT) {}

Map::Map(int width, int height) : width(width), height(height) {
	seed = TCODRandom::getInstance()->getInt(0, std::numeric_limits<int>::max());
}

Map::Map(int width, int height, long seed) : width(width), height(height), seed(seed) {
}

Map::~Map() {
}

void Map::init(bool initActors) {
	TCODRandom rng(seed);
	for(int i = 0; i < width*height; ++i) {
		tiles.push_back(Tile());
	}
	this->map = std::make_shared<TCODMap>(width, height);
	TCODBsp bsp(0, 0, width, height);
	// 8: recursion level,
	// 1.5f, 1.5f H/V and V/H ratios of rooms
	bsp.splitRecursive(&rng, 8, ROOM_MAX_SIZE, ROOM_MAX_SIZE, 1.5f, 1.5f);
	BspListener listener(*this, &rng);
	bsp.traverseInvertedLevelOrder(&listener, (void*) initActors);
}

Actor* Map::makeMonster(int x, int y) {
	int r = d100();
	if(r < 70) {
		Actor* punk = new Actor(x, y, 'h', "punk", TCODColor::desaturatedGreen, 1); // TODO
		punk->destructible = std::make_unique<MonsterDestructible>(10, 0, 50, "dead punk", 13);
		punk->attacker = std::make_unique<Attacker>(3);
		punk->ai = std::make_unique<MonsterAi>();
		return punk;
	} else if (r < 80) {
		Actor* fighter = new Actor(x, y, 'H', "fighter", TCODColor::darkerGreen, 1); // TODO
		fighter->destructible = std::make_unique<MonsterDestructible>(16, 1, 100, "fighter carcass", 15);
		fighter->attacker = std::make_unique<Attacker>(4);
		fighter->ai = std::make_unique<MonsterAi>();
		return fighter;
	}
	else {
		Actor* boxer = new Actor(x, y, 'H', "boxer", TCODColor::darkestGreen, 1); // TODO
		boxer->destructible = std::make_unique<MonsterDestructible>(4, 1, 70, "boxer carcass", 16);
		boxer->attacker = std::make_unique<Attacker>(4);
		boxer->ai = std::make_unique<MonsterAi>();
		return boxer;
	}
}

void Map::addMonster(int x, int y) {
	state->addActor(makeMonster(x, y));
}

Actor* Map::makeItem(int x, int y) {
	int r = d100();
	if(r < 60) {
		Actor* stimpak = new Actor(x, y, '!', "stimpak", TCODColor::violet);
		stimpak->blocks = false;
		stimpak->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::WEARER, 0), std::make_unique<HealthEffect>(4));
		return stimpak;
	} else if(r < 70) {
		Actor* blasterBoltDevice = new Actor(x, y, '?', "blaster bolt device", TCODColor::lightYellow);
		blasterBoltDevice->blocks = false;
		blasterBoltDevice->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::CLOSEST_MONSTER, 5), std::make_unique<HealthEffect>(-20, "The %s is hit by a blast!\n The damage is %g hit points."));
		return blasterBoltDevice;
	} else if(r < 80) {
		Actor* fragGrenade = new Actor(x, y, '?', "fragmentation grenade", TCODColor::lightGreen);
		fragGrenade->blocks = false;
		fragGrenade->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::SELECTED_RANGE, 3), std::make_unique<HealthEffect>(-12, "The grenade explodes, hurting the %s for %g hit points!"));
		return fragGrenade;
	} else if(r < 90) {
		Actor* confusor = new Actor(x, y, '?', "confusor", TCODColor::lightBlue);
		confusor->blocks = false;
		confusor->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::SELECTED_MONSTER, 5), std::make_unique<AiChangeEffect>(std::make_unique<ConfusedMonsterAi>(10), "The eyes of the %s look vacant!"));
		return confusor;
	} else {
		Actor* teslaCoil = new Actor(x, y, '#', "tesla coil", TCODColor::lightBlue);
		teslaCoil->blocks = false;
		teslaCoil->pickable = std::make_unique<Pickable>(TargetSelector(TargetSelector::SelectorType::WEARER_RANGE, 5), std::make_unique<HealthEffect>(-6, "The tesla coil sputters, emitting raw\n electricity, hurting %s for %g hp!"));
		return teslaCoil;
	}
}

void Map::addItem(int x, int y) {
	state->addActor(makeItem(x, y));
}

bool Map::isWall(int x, int y) const {
	return !map->isWalkable(x, y);
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
	if(tiles[x+y*width].newInFov) {
		return true;
	}
	return false;
	//return isInFovNew(x, y);
}

void Map::computeFov(FovType fovType) {
	for(int x = 0; x < width; x++) {
		for(int y = 0; y < height; y++) {
			tiles[x + width*y].newInFov = false;
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
		// TODO break at map boundaries
		for(int col = 0; col <= row; col++) {
			// TODO break to this direction, too
			if(fovType == FovType::CIRCLE) {
				if(sqrt(row*row + col*col) > constants::DEFAULT_FOV_RADIUS) break;
			} // else if fovType == FovType::SQUARE
			int xPos = x + transformOctant(row, col, octant).x;
			int yPos = y + transformOctant(row, col, octant).y;
			if(fullShadow) {
				tiles[xPos + width*yPos].newInFov = false;
			}
			else {
				Shadow projection = Shadow::projectTile(row, col);
				bool visible = !shadowLine.isInShadow(projection);
				tiles[xPos + width*yPos].newInFov = visible;
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

void Map::dig(int x1, int y1, int x2, int y2) {
	if(x2 < x1) std::swap(x1, x2);
	if(y2 < y1) std::swap(y1, y2);
	for(int tileX = x1; tileX <= x2; ++tileX) {
		for(int tileY = y1; tileY <= y2; ++tileY) {
			map->setProperties(tileX, tileY, true, true);
		}
	}
}

void Map::createRoom(bool first, int x1, int y1, int x2, int y2, bool initActors) {
	dig (x1, y1, x2, y2);
	if(!initActors) { return; }
	if(first) {
		// put the player in the first room
		state->getPlayer()->x=(x1 + x2)/2;
		state->getPlayer()->y=(y1 + y2)/2;
		// state->getStairs()->x = (x1 + x2) / 2 + 1; // debugging
		// state->getStairs()->y = (y1 + y2) / 2;
	} else {
		int nMonsters = rng.getInt(0, MAX_ROOM_MONSTERS);
		while(nMonsters > 0) {
			int x = rng.getInt(x1, x2);
			int y = rng.getInt(y1, y2);
			if(canWalk(x, y)) addMonster(x, y);
			--nMonsters;
		}
	}
	// add stimpaks
	int nItems = rng.getInt(0, MAX_ROOM_ITEMS);
	while (nItems > 0) {
		int x = rng.getInt(x1, x2);
		int y = rng.getInt(y1, y2);
		if(canWalk(x,y)) { addItem(x,y); }
		--nItems;
	}
	state->getStairs()->x = (x1 + x2) / 2;
	state->getStairs()->y = (y1 + y2) / 2;
}

BspListener::BspListener(Map &map, TCODRandom* rng) : roomNum(0), map(map), rng(rng) {;}

bool BspListener::visitNode(TCODBsp* node, void* userData) {
	bool initActors = (bool) userData;
	if(node->isLeaf()) {
		int x;
		int y;
		int w;
		int h;
		// dig a room
		w = rng->getInt(ROOM_MIN_SIZE, node->w-2);
		h = rng->getInt(ROOM_MIN_SIZE, node->h-2);
		x = rng->getInt(node->x+1, node->x+node->w-w-1);
		y = rng->getInt(node->y+1, node->y+node->h-h-1);
		map.createRoom(roomNum == 0, x, y, x+w-1, y+h-1, initActors);
		if(roomNum != 0) {
			// dig a corridor from last room
			map.dig(lastx, lasty, x+w/2, lasty);
			map.dig(x+w/2, lasty, x+w/2, y+h/2);
		}
		lastx = x+w/2;
		lasty = y+h/2;
		++roomNum;
	}
return true;
}
