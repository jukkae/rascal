#include <limits>
#include "libtcod.hpp"
#include "main.hpp"

Engine::Engine(int screenWidth, int screenHeight) :
gameStatus(GameStatus::STARTUP), fovRadius(10), screenWidth(screenWidth), screenHeight(screenHeight), level(1) {
	TCODConsole::initRoot(80, 50, "Rascal", false);
}

Engine::~Engine() {
	term();
}

// clear actors, map and log
void Engine::term() {
	actors.clear();
	if(map) delete map;
	gui.clear();
}

void Engine::init() {
	player = new Actor(40, 25, '@', "you", TCODColor::white);
	player->destructible = new PlayerDestructible(30, 2, 0, "your corpse");
	player->attacker = std::unique_ptr<Attacker>(new Attacker(5));
	player->ai = new PlayerAi();
	player->container = new Container(26);
	actors.push_back(player);
	stairs = new Actor(0, 0, '>', "stairs", TCODColor::white);
	stairs->blocks = false;
	stairs->fovOnly = false;
	actors.push_back(stairs);
	map = new Map(80, 43);
	map->init(true);
	gui.message(TCODColor::green, "Welcome to year 20XXAD, you strange rascal!\nPrepare to fight or die!");
	gameStatus = GameStatus::STARTUP;
}

void Engine::update() {
	if(gameStatus == GameStatus::STARTUP) map->computeFov();
	gameStatus = GameStatus::IDLE;
	player->update();
	map->markExploredTiles();
	if(gameStatus == GameStatus::NEW_TURN) {
		for(auto iterator = actors.begin(); iterator != actors.end(); iterator++) {
			Actor* actor = *iterator;
			if (actor != player) actor->update();
		}
	}
}

void Engine::render() {
	TCODConsole::root->clear();
	map->render();
	for (auto iterator=actors.begin(); iterator != actors.end(); iterator++) {
		Actor *actor = *iterator;
		if(actor != player && ((!actor->fovOnly && map->isExplored(actor->x, actor->y)) || map->isInFov(actor->x, actor->y))) {
			actor->render();
		}
	}
	player->render();
	gui.render();
}

// TODO sometimes segfaults
void Engine::nextLevel() {
	level++;
	gui.message(TCODColor::lightViolet,"You take a moment to rest, and recover your strength.");
	player->destructible->heal(player->destructible->maxHp/2);
	gui.message(TCODColor::red,"After a rare moment of peace, you descend\ndeeper into the heart of the dungeon...");
	delete map;
	for (auto it=actors.begin(); it!=actors.end(); it++) {
       if (*it != player && *it != stairs) {
           delete *it;
           it = actors.erase(it);
       }
   }
   map = new Map(80,43);
   map->init(true);
   gameStatus = GameStatus::STARTUP;
}

void Engine::sendToBack(Actor* actor) {
	actors.erase(std::remove(actors.begin(), actors.end(), actor), actors.end());
	actors.insert(actors.begin(), actor);
}

Actor* Engine::getClosestMonster(int x, int y, float range) const {
	Actor *closest = NULL;
	float bestDistance = std::numeric_limits<float>::max();
	for (auto i = actors.begin(); i != actors.end(); i++) {
		Actor* actor = *i;
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
	for(auto i = actors.begin(); i != actors.end(); i++) {
		Actor* actor = *i;
		if(actor->x == x && actor->y == y && actor->destructible && !actor->destructible->isDead()) return actor;
	}
	return NULL;
}

bool Engine::pickTile(int* x, int* y, float maxRange) {
	while(!TCODConsole::isWindowClosed()) {
		render();
		for(int cx = 0; cx < map->width; cx++) {
			for(int cy = 0; cy < map->height; cy++) {
				if(map->isInFov(cx, cy) && (maxRange == 0 || player->getDistance(cx, cy) <= maxRange)) {
					TCODColor col = TCODConsole::root->getCharBackground(cx, cy);
					col = col * 1.2;
					TCODConsole::root->setCharBackground(cx, cy, col);
				}
			}
		}
		TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &lastKey, &mouse, true);
		if(map->isInFov(mouse.cx, mouse.cy) && (maxRange == 0 || player->getDistance(mouse.cx, mouse.cy) <= maxRange)) {
			TCODConsole::root->setCharBackground(mouse.cx,mouse.cy,TCODColor::white);
			if(mouse.lbutton_pressed) {
				*x = mouse.cx;
				*y = mouse.cy;
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
