#include <limits>
#include "libtcod.hpp"
#include "actor.hpp"
#include "ai.hpp"
#include "container.hpp"
#include "destructible.hpp"
#include "engine.hpp"

Engine::Engine(int screenWidth, int screenHeight) :
gameStatus(GameStatus::STARTUP), fovRadius(10), screenWidth(screenWidth), screenHeight(screenHeight), level(1), time(0) {
	TCODConsole::initRoot(80, 50, "Rascal", false);
}

Engine::~Engine() {
	term();
}

// clear actors, map and log
void Engine::term() {
	actors.clear();
	gui.clear();
}

void Engine::init() {
	player               = new Actor(40, 25, '@', "you", TCODColor::white);
	player->destructible = std::unique_ptr<Destructible>(new PlayerDestructible(30, 2, 0, "your corpse"));
	player->attacker     = std::unique_ptr<Attacker>(new Attacker(5));
	player->ai           = std::unique_ptr<Ai>(new PlayerAi());
	player->container    = std::unique_ptr<Container>(new Container(26));
	actors.push_back(player);

	stairs = new Actor(0, 0, '>', "stairs", TCODColor::white);
	stairs->blocks = false;
	stairs->fovOnly = false;
	actors.push_back(stairs);

	map = std::unique_ptr<Map>(new Map(120, 72));
	map->init(true);

	time = 0;

	gui.message(TCODColor::green, "Welcome to year 20XXAD, you strange rascal!\nPrepare to fight or die!");
	updateQueue();
	gameStatus = GameStatus::STARTUP;
}

void Engine::updateQueue() {
	actorsQueue.clear();
	for(int i = 0; i < actors.size(); i++) {
		Actor* a = actors.at(i);
		if(a->ai) {
			actorsQueue.push_back(std::pair<float, Actor*>((100.0+i), a)); // TODO wait... why the 100.0+i here?
		}
	}
}

void Engine::update() {
	if(gameStatus == GameStatus::STARTUP) map->computeFov();

	gameStatus = GameStatus::NEW_TURN;
	if (gameStatus == GameStatus::NEW_TURN) {
		std::pair<float, Actor*>& activeActor = actorsQueue.at(0);
		float activeActorTUNA = activeActor.first;
		for(std::pair<float, Actor*>& a : actorsQueue) {
			a.first -= activeActorTUNA;
		}
		if(activeActor.second == player) {
			render();
		}
		float elapsedTime = activeActor.second->update();
		if(activeActor.second == player) {
			time += elapsedTime;
			map->markExploredTiles();
			render();
		}
		activeActor.first += elapsedTime;
		std::sort(actorsQueue.begin(), actorsQueue.end());
	}
}

void Engine::render() {
	TCODConsole::root->clear();
	renderMap(*map);
	for (Actor* actor : actors) {
		if(actor != player && ((!actor->fovOnly && map->isExplored(actor->x, actor->y)) || map->isInFov(actor->x, actor->y))) {
			renderActor(*actor);
		}
	}
	renderActor(*player);
	gui.render();
}

void Engine::renderMap(const Map& map) const {
	static const TCODColor darkWall   (0, 0, 100);
	static const TCODColor darkGround (50, 50, 150);
	static const TCODColor lightWall  (130, 110, 50);
	static const TCODColor lightGround(200, 180, 50);
	int cameraX = player->x - (screenWidth/2); // upper left corner of camera
	int cameraY = player->y - (screenHeight/2);

	for(int x = 0; x < map.width; x++) { // TODO no need to loop through all of map, just what's visible
		for(int y = 0; y < map.height; y++) {
			int screenX = x - cameraX;
			int screenY = y - cameraY;
			if(map.isInFov(x, y)) {
				TCODConsole::root->setCharBackground(screenX, screenY, map.isWall(x,y) ? lightWall : lightGround);
			}
			else if(map.isExplored(x, y)) {
				TCODConsole::root->setCharBackground(screenX, screenY, map.isWall(x,y) ? darkWall : darkGround);
			}
		}
	}
}

void Engine::renderActor(const Actor& actor) const {
	int cameraX = player->x - (screenWidth/2); // upper left corner of camera
	int cameraY = player->y - (screenHeight/2);
	int x = actor.x - cameraX;
	int y = actor.y - cameraY;

	TCODConsole::root->setChar(x, y, actor.ch);
	TCODConsole::root->setCharForeground(x, y, actor.col);
}

void Engine::nextLevel() {
	level++;
	gui.message(TCODColor::lightViolet,"You take a moment to rest, and recover your strength.");
	player->destructible->heal(player->destructible->maxHp/2);
	gui.message(TCODColor::red,"After a rare moment of peace, you descend\ndeeper into the heart of the dungeon...");

	// Clunky, not idiomatic
	auto it = actors.begin();
	while (it != actors.end()) {
       if (*it != player && *it != stairs) {
           it = actors.erase(it);
       }
	   else ++it;
   }

   map = std::unique_ptr<Map>(new Map(80,43));
   map->init(true);

   updateQueue(); // A bit hacky

   gameStatus = GameStatus::STARTUP;
}

void Engine::sendToBack(Actor* actor) {
	actors.erase(std::remove(actors.begin(), actors.end(), actor), actors.end());
	actors.insert(actors.begin(), actor);
}

Actor* Engine::getClosestMonster(int x, int y, float range) const {
	Actor* closest = nullptr;
	float bestDistance = std::numeric_limits<float>::max();
	for (Actor* actor : actors) {
		if(actor != player && actor->destructible && !actor->destructible->isDead()) {
			float distance = actor->getDistance(x,y);
			if(distance < bestDistance && (distance <= range || range == 0.0f)) {
				bestDistance = distance;
				closest = actor;
			}
		}
	}
	return closest;
}

Actor* Engine::getLiveActor(int x, int y) const {
	for(Actor* actor : actors) {
		if(actor->x == x && actor->y == y && actor->destructible && !actor->destructible->isDead()) return actor;
	}
	return nullptr;
}

// this is a mess. a working mess, sure, but a mess nonetheless.
bool Engine::pickTile(int* x, int* y, float maxRange) {
	while(!TCODConsole::isWindowClosed()) {
		render();
		for(int cx = 0; cx < map->width; cx++) {
			for(int cy = 0; cy < map->height; cy++) {
				int realX = cx - (screenWidth / 2) + player->x; // real good naming here, har har
				int realY = cy - (screenHeight / 2) + player->y;
				if(map->isInFov(realX, realY) && (maxRange == 0 || player->getDistance(realX, realY) <= maxRange)) {
					TCODColor col = TCODConsole::root->getCharBackground(cx, cy);
					col = col * 1.2;
					TCODConsole::root->setCharBackground(cx, cy, col);
				}
			}
		}
		TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &lastKey, &mouse, true);
		int realX = mouse.cx - (screenWidth / 2) + player->x;
		int realY = mouse.cy - (screenHeight / 2) + player->y;
		if(map->isInFov(realX, realY) && (maxRange == 0 || player->getDistance(realX, realY) <= maxRange)) {
			TCODConsole::root->setCharBackground(mouse.cx,mouse.cy,TCODColor::white);
			if(mouse.lbutton_pressed) {
				*x = realX;
				*y = realY;
				return true;
			}
		}
		if(mouse.rbutton_pressed || lastKey.vk != TCODK_NONE) {
			return false;
		}
		TCODConsole::flush();
	}
	return false;
}
