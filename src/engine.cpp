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
	gameplayState.initMap();

	states.push_back(&dummyState);
	states.push_back(&gameplayState);
}

void Engine::update() {
	states.back()->update(this);
}

void Engine::render() {
	states.back()->render(this);
}
