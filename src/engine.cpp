#include "libtcod.hpp"
#include "actor.hpp"
#include "map.hpp"
#include "engine.hpp"

Engine::Engine() : gameStatus(STARTUP), fovRadius(10) {
	TCODConsole::initRoot(80, 50, "Rascal", false);
	player = new Actor(40, 25, '@', "player", TCODColor::white);
	actors.push(player);
	map = new Map(80, 45);
}

Engine::~Engine() {
	actors.clearAndDelete();
	delete map;
}

void Engine::update() {
	TCOD_key_t key;
	if(gameStatus == STARTUP) map->computeFov();
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL);
	int dx = 0;
	int dy = 0;
	switch(key.vk) {
		case TCODK_UP:
			dy = -1;
			break;
		case TCODK_DOWN:
			dy = 1;
			break;
		case TCODK_LEFT:
			dx = -1;
			break;
		case TCODK_RIGHT:
			dx = 1;
			break;
		default: break;
	}
	if(dx != 0 || dy != 0) {
		gameStatus = NEW_TURN;
		if(player->moveOrAttack(player->x + dx, player->y + dy)) map->computeFov();
	}
	if(gameStatus == NEW_TURN) {
		for(Actor** iterator = actors.begin(); iterator != actors.end(); iterator++) {
			Actor* actor = *iterator;
			if(actor != player) {
				actor->update();
			}
		}
		gameStatus = IDLE;
	}
}


void Engine::render() {
	TCODConsole::root->clear();
	map->render();
	for (Actor **iterator=actors.begin(); iterator != actors.end(); iterator++) {
		Actor *actor = *iterator;
		if(map->isInFov(actor->x, actor->y)) {
			actor->render();
		}
	}
}
