#include <algorithm>
#include <limits>
#include "libtcod.hpp"
#include "ai.hpp"
#include "constants.hpp"
#include "container.hpp"
#include "destructible.hpp"
#include "engine.hpp"

Engine::Engine(int dummy) {
	TCODConsole::initRoot(constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT, "Rascal", false);
}

Engine::~Engine() {
	term();
}

void Engine::term() {
	gui.clear();
}

void Engine::init() {
	gameplayState.init();

	player = gameplayState.getPlayer();

	gameplayState.initMap();

	gui.message(TCODColor::green, "Welcome to year 20XXAD, you strange rascal!\nPrepare to fight or die!");

	states.push_back(&dummyState);
	states.push_back(&gameplayState);
}

void Engine::update() {
	states.back()->update(this);
}

void Engine::render() {
	states.back()->render(this);
}

// this is a mess. a working mess, sure, but a mess nonetheless.
bool Engine::pickTile(int* x, int* y, float maxRange) {
	while(!TCODConsole::isWindowClosed()) {
		render();
		for(int cx = 0; cx < constants::SCREEN_WIDTH; ++cx) {
			for(int cy = 0; cy < constants::SCREEN_HEIGHT; ++cy) {
				Point location = renderer.getWorldCoordsFromScreenCoords(Point(cx, cy));
				int realX = location.x;
				int realY = location.y;
				if(gameplayState.isInFov(realX, realY) && (maxRange == 0 || player->getDistance(realX, realY) <= maxRange)) {
					TCODColor col = TCODConsole::root->getCharBackground(cx, cy);
					col = col * 1.2;
					TCODConsole::root->setCharBackground(cx, cy, col);
				}
			}
		}
		TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &lastKey, &mouse, true);
		Point mouseLocation = renderer.getWorldCoordsFromScreenCoords(Point(mouse.cx, mouse.cy));
		int realX = mouseLocation.x;
		int realY = mouseLocation.y;
		if(gameplayState.isInFov(realX, realY) && (maxRange == 0 || player->getDistance(realX, realY) <= maxRange)) {
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
