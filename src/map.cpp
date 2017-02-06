#include "libtcod.hpp"
#include "map.hpp"
#include "actor.hpp"
#include "engine.hpp"
#include <algorithm>

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;

Map::Map(int width, int height) : width(width), height(height) {
	tiles = new Tile[width*height];
	TCODBsp bsp(0, 0, width, height);
	// 8: recursion level,
	// 1.5f, 1.5f H/V and V/H ratios of rooms
	bsp.splitRecursive(NULL, 8, ROOM_MAX_SIZE, ROOM_MAX_SIZE, 1.5f, 1.5f);
	BspListener listener(*this);
	bsp.traverseInvertedLevelOrder(&listener, NULL);
}

Map::~Map() {
	delete [] tiles;
}

bool Map::isWall(int x, int y) const {
	return !tiles[x + y*width].canWalk;
}

void Map::setWall(int x, int y) {
	tiles[x + y*width].canWalk = false;
}

void Map::render() const {
	static const TCODColor darkWall(0, 0, 100);
	static const TCODColor darkGround(50, 50, 150);

	for(int x = 0; x < width; x++) {
		for(int y = 0; y < height; y++) {
			TCODConsole::root->setCharBackground(x, y, isWall(x,y) ? darkWall : darkGround);
		}
	}
}

void Map::dig(int x1, int y1, int x2, int y2) {
	if(x2 < x1) std::swap(x1, x2);
	if(y2 < y1) std::swap(y1, y2);
	for(int tileX = x1; tileX <= x2; tileX++) {
		for(int tileY = y1; tileY <= y2; tileY++) {
			tiles[tileX + tileY*width].canWalk = true;
		}
	}
}

void Map::createRoom(bool first, int x1, int y1, int x2, int y2) {
	dig (x1, y1, x2, y2);
	if(first) {
		// put the player in the first room
		engine.player->x=(x1+x2)/2;
		engine.player->y=(y1+y2)/2;
	} else {
		TCODRandom* rng = TCODRandom::getInstance();
		if (rng->getInt(0,3) == 0) {
			engine.actors.push(new Actor((x1+x2)/2, (y1+y2)/2, '@', TCODColor::yellow));
		}
	}
}

BspListener::BspListener(Map &map) : map(map), roomNum(0) {;}

bool BspListener::visitNode(TCODBsp *node, void *userData) {
	if ( node->isLeaf() ) {
		int x,y,w,h;
		// dig a room
		TCODRandom *rng = TCODRandom::getInstance();
		w = rng->getInt(ROOM_MIN_SIZE, node->w-2);
		h = rng->getInt(ROOM_MIN_SIZE, node->h-2);
		x = rng->getInt(node->x+1, node->x+node->w-w-1);
		y = rng->getInt(node->y+1, node->y+node->h-h-1);
		map.createRoom(roomNum == 0, x, y, x+w-1, y+h-1);
		if ( roomNum != 0 ) {
			// dig a corridor from last room
			map.dig(lastx, lasty, x+w/2, lasty);
			map.dig(x+w/2, lasty, x+w/2, y+h/2);
		}
		lastx = x+w/2;
		lasty = y+h/2;
		roomNum++;
	}
return true;
}
