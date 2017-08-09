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
}

void Engine::init() {
	gameplayState.init();
	mainMenuState.init();
	states.push_back(&dummyState);
	states.push_back(&gameplayState);
	states.push_back(&mainMenuState);
}

void Engine::update() {
	TCOD_key_t key;
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, nullptr, false); // blocks, nasty, but will work for now
	switch(key.vk) {
		case TCODK_UP: std::cout << "up\n";
		default: break;
	}

	states.back()->update(this);
}

void Engine::render() {
	states.back()->render(this);
}
