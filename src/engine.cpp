#include "libtcod.hpp"
#include "main.hpp"

Engine::Engine(int screenWidth, int screenHeight) :
gameStatus(STARTUP), fovRadius(10), screenWidth(screenWidth), screenHeight(screenHeight) {
	TCODConsole::initRoot(80, 50, "Rascal", false);
	gui = new Gui();
}

Engine::~Engine() {
	actors.clearAndDelete();
	delete map;
	delete gui;
}

void Engine::init() {
	player = new Actor(40, 25, '@', "you", TCODColor::white);
	player->destructible = new PlayerDestructible(30, 2, "your corpse");
	player->attacker = new Attacker(5);
	player->ai = new PlayerAi();
	player->container = new Container(26);
	actors.push(player);
	map = new Map(80, 43);
	map->init();
	gui->message(TCODColor::green, "Welcome to year 20XXAD, you strange rascal!\nPrepare to fight or die!");
}

void Engine::save() {
	if (player->destructible->isDead()) { // Permadeath
		TCODSystem::deleteFile("game.sav");
	} else {
		TCODZip zip;
		zip.putInt(map->width);
		zip.putInt(map->height);
		map->save(zip);
		player->save(zip);
		/* TODO save other actors
		zip.putInt(actors.size()-1);
		for (Actor **it=actors.begin(); it!=actors.end(); it++) {
			if ( *it != player ) {
				(*it)->save(zip);
			}
		}
		gui->save(zip);
		*/
		zip.saveToFile("game.sav");
	}
}

void Engine::load() {
	if(TCODSystem::fileExists("game.sav")) {
		player = new Actor(40, 25, '@', "you", TCODColor::white);
		player->destructible = new PlayerDestructible(30, 2, "your corpse");
		player->attacker = new Attacker(5);
		player->ai = new PlayerAi();
		player->container = new Container(26);
		actors.push(player);
		TCODZip zip;
		zip.loadFromFile("game.sav");
		int width = zip.getInt();
		int height = zip.getInt();
		map = new Map(width, height);
		map->load(zip);
		player->load(zip);
	} else {
		engine.init();
	}
}

void Engine::update() {
	if(gameStatus == STARTUP) map->computeFov();
	gameStatus = IDLE;
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE, &lastKey, &mouse);
	player->update();
	if(gameStatus == NEW_TURN) {
		for(Actor** iterator = actors.begin(); iterator != actors.end(); iterator++) {
			Actor* actor = *iterator;
			if (actor != player) actor->update();
		}
	}
}


void Engine::render() {
	TCODConsole::root->clear();
	map->render();
	for (Actor **iterator=actors.begin(); iterator != actors.end(); iterator++) {
		Actor *actor = *iterator;
		if(actor != player && map->isInFov(actor->x, actor->y)) {
			actor->render();
		}
	}
	player->render();
	gui->render();
	TCODConsole::root->print(1, screenHeight-2, "HP : %d/%d", (int) player->destructible->hp, (int) player->destructible->maxHp);
}

void Engine::sendToBack(Actor* actor) {
	actors.remove(actor);
	actors.insertBefore(actor, 0);
}

Actor* Engine::getClosestMonster(int x, int y, float range) const {
	Actor *closest = NULL;
	float bestDistance = 1E6f; // TODO float max
	for (Actor** i = actors.begin(); i != actors.end(); i++) {
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

Actor* Engine::getActor(int x, int y) const { // TODO actually, should be getLiveActor or something like that
	for(Actor** i = actors.begin(); i != actors.end(); i++) {
		Actor* actor = *i;
		if(actor->x == x && actor->y == y && actor->destructible && !actor->destructible->isDead()) return actor;
	}
	return NULL;
}

bool Engine::pickTile(int* x, int* y, float maxRange) { // TODO move to keyboard picking
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
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &lastKey, &mouse);
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
