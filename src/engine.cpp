#include <limits>
#include "libtcod.hpp"
#include "actor.hpp"
#include "ai.hpp"
#include "container.hpp"
#include "destructible.hpp"
#include "engine.hpp"

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

	map = std::unique_ptr<Map>(new Map(80, 43));
	map->init(true);

	gui.message(TCODColor::green, "Welcome to year 20XXAD, you strange rascal!\nPrepare to fight or die!");
	updateQueue();
	gameStatus = GameStatus::STARTUP;
}

void Engine::updateQueue() {
	actorsQueue.clear();
	for(int i = 0; i < actors.size(); i++) {
		Actor* a = actors.at(i);
		if(a->ai) {
			actorsQueue.push_back(std::pair<float, Actor*>((100.0+i), a));
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
			map->markExploredTiles();
			render();
		}
		activeActor.first += elapsedTime;
		std::sort(actorsQueue.begin(), actorsQueue.end());
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

void Engine::nextLevel() {
	level++;
	gui.message(TCODColor::lightViolet,"You take a moment to rest, and recover your strength.");
	player->destructible->heal(player->destructible->maxHp/2);
	gui.message(TCODColor::red,"After a rare moment of peace, you descend\ndeeper into the heart of the dungeon...");

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
	Actor *closest = nullptr;
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
	return nullptr;
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
