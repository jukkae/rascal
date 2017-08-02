#include <algorithm>
#include <limits>
#include "actor.hpp"
#include "attacker.hpp"
#include "destructible.hpp"
#include "engine.hpp"
#include "map.hpp"
#include "pickable.hpp"

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;
static const int MAX_ROOM_MONSTERS = 3;
static const int MAX_ROOM_ITEMS = 2;
static const int fovRadius = 10; // TODO doesn't belong here

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
	this->map = std::unique_ptr<TCODMap>(new TCODMap(width, height));
	TCODBsp bsp(0, 0, width, height);
	// 8: recursion level,
	// 1.5f, 1.5f H/V and V/H ratios of rooms
	bsp.splitRecursive(&rng, 8, ROOM_MAX_SIZE, ROOM_MAX_SIZE, 1.5f, 1.5f);
	BspListener listener(*this, &rng);
	bsp.traverseInvertedLevelOrder(&listener, (void*) initActors);
}

Actor* Map::makeMonster(int x, int y) {
	if(rng.getInt(0, 100) < 80) {
		Actor* punk = new Actor(x, y, 'h', "punk", TCODColor::desaturatedGreen, 1); // TODO
		punk->destructible = std::unique_ptr<Destructible>(new MonsterDestructible(10, 0, 50, "dead punk"));
		punk->attacker = std::unique_ptr<Attacker>(new Attacker(3));
		punk->ai = std::unique_ptr<Ai>(new MonsterAi());
		return punk;
	} else {
		Actor* fighter = new Actor(x, y, 'H', "fighter", TCODColor::darkerGreen, 1); // TODO
		fighter->destructible = std::unique_ptr<Destructible>(new MonsterDestructible(16, 1, 100, "fighter carcass"));
		fighter->attacker = std::unique_ptr<Attacker>(new Attacker(4));
		fighter->ai = std::unique_ptr<Ai>(new MonsterAi());
		return fighter;
	}
}

void Map::addMonster(int x, int y) {
	engine.actors.push_back(makeMonster(x, y));
}

Actor* Map::makeItem(int x, int y) {
	int r = rng.getInt(0, 100);
	if(r < 60) {
		Actor* stimpak = new Actor(x, y, '!', "stimpak", TCODColor::violet);
		stimpak->blocks = false;
		stimpak->pickable = std::unique_ptr<Pickable>(new Pickable(TargetSelector(TargetSelector::SelectorType::WEARER, 0), std::unique_ptr<Effect>(new HealthEffect(4))));
		return stimpak;
	} else if(r < 70) {
		Actor* blasterBoltDevice = new Actor(x, y, '?', "blaster bolt device", TCODColor::lightYellow);
		blasterBoltDevice->blocks = false;
		blasterBoltDevice->pickable = std::unique_ptr<Pickable>(new Pickable(TargetSelector(TargetSelector::SelectorType::CLOSEST_MONSTER, 5), std::unique_ptr<Effect>(new HealthEffect(-20, "The %s is hit by a blast!\n The damage is %g hit points."))));
		return blasterBoltDevice;
	} else if(r < 80) {
		Actor* fragGrenade = new Actor(x, y, '?', "fragmentation grenade", TCODColor::lightGreen);
		fragGrenade->blocks = false;
		fragGrenade->pickable = std::unique_ptr<Pickable>(new Pickable(TargetSelector(TargetSelector::SelectorType::SELECTED_RANGE, 3), std::unique_ptr<Effect>(new HealthEffect(-12, "The grenade explodes, hurting the %s for %g hit points!"))));
		return fragGrenade;
	} else if(r < 90) {
		Actor* confusor = new Actor(x, y, '?', "confusor", TCODColor::lightBlue);
		confusor->blocks = false;
		confusor->pickable = std::unique_ptr<Pickable>(new Pickable(TargetSelector(TargetSelector::SelectorType::SELECTED_MONSTER, 5), std::unique_ptr<Effect>(new AiChangeEffect(std::unique_ptr<TemporaryAi>(new ConfusedMonsterAi(10)), "The eyes of the %s look vacant!"))));
		return confusor;
	} else {
		Actor* teslaCoil = new Actor(x, y, '#', "tesla coil", TCODColor::lightBlue);
		teslaCoil->blocks = false;
		teslaCoil->pickable = std::unique_ptr<Pickable>(new Pickable(TargetSelector(TargetSelector::SelectorType::WEARER_RANGE, 5), std::unique_ptr<Effect>(new HealthEffect(-6, "The tesla coil sputters, emitting raw\n electricity, hurting %s for %g hp!"))));
		return teslaCoil;
	}
}

void Map::addItem(int x, int y) {
	engine.actors.push_back(makeItem(x, y));
}

bool Map::isWall(int x, int y) const {
	return !map->isWalkable(x, y);
}

bool Map::canWalk(int x, int y) const {
	if(isWall(x, y)) return false;
	for(Actor* actor : engine.actors) {
		if(actor->blocks && actor->x == x && actor->y == y) {
			return false;
		}
	}
	return true;
}

bool Map::isExplored(int x, int y) const {
	return tiles[x + y*width].explored;
}

bool Map::isInFov(int x, int y) const {
	if(x < 0 || x >= width || y < 0 || y >= height) return false;
	if(map->isInFov(x, y)) {
		return true;
	}
	return false;
}

void Map::markAsExplored(int x, int y) {
	if(map->isInFov(x, y)) tiles[x + y*width].explored = true;
}

void Map::markExploredTiles() {
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			if(isInFov(x, y)) markAsExplored(x, y);
		}
	}
}

void Map::computeFov() {
	map->computeFov(engine.getPlayer()->x, engine.getPlayer()->y, fovRadius);
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
		engine.getPlayer()->x=(x1 + x2)/2;
		engine.getPlayer()->y=(y1 + y2)/2;
		// engine.stairs->x = (x1 + x2) / 2 + 1; // debugging
		// engine.stairs->y = (y1 + y2) / 2;
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
	engine.stairs->x = (x1 + x2) / 2;
	engine.stairs->y = (y1 + y2) / 2;
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
