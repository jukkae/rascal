#include <algorithm>
#include <limits>
#include "libtcod.hpp"
#include "actor.hpp"
#include "ai.hpp"
#include "constants.hpp"
#include "container.hpp"
#include "destructible.hpp"
#include "engine.hpp"

Engine::Engine(int dummy) : gameStatus(GameStatus::STARTUP) {
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

	std::sort(actors->begin(), actors->end(), [](const auto& lhs, const auto& rhs)
	{
		return lhs->energy > rhs->energy;
	});

	gui.message(TCODColor::green, "Welcome to year 20XXAD, you strange rascal!\nPrepare to fight or die!");
	gameStatus = GameStatus::STARTUP;

	states.push_back(&dummyState);
	states.push_back(&gameplayState);
}

void Engine::update() {
	states.back()->update(this);

	if(gameStatus == GameStatus::STARTUP) gameplayState.computeFov();

	gameStatus = GameStatus::NEW_TURN;
	if (gameStatus == GameStatus::NEW_TURN) {
		render(); // yep rendering is that cheap now
		Actor* activeActor = getNextActor();
		if(activeActor->isPlayer()) {
			//render();
		}
		updateNextActor();
		if(activeActor->isPlayer()) {
			gameplayState.markExploredTiles();
			render();
		}
	}
	if(gameStatus == GameStatus::DEFEAT) {
		// TODO u ded
	}
}

void Engine::updateNextActor() {
	gameplayState.updateNextActor();
	updateTime();
}

void Engine::updateTime() {
	if(actors->at(0)->energy.get() > 0) return; // TODO check if energy HAS value (if(a->energy))
	else {
		Actor* next = *std::find_if(actors->begin(), actors->end(), [](const auto& a) { return a->ai != nullptr; });

		float tuna = next->energy.get() * -1;

		gameplayState.increaseTime(tuna); // TODO can't rely on explicitly pointing to gameplayState

		for(auto a : *actors) {
			if(a->ai != nullptr) *a->energy += tuna;
		}
	}
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
