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
	init();
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
	while(commands.size() > 0) executeCommand();

	states.back()->handleEvents(this);
	states.back()->update(this);
	states.back()->render(this);
}

void Engine::executeCommand() {
	commands.front()->execute();
	commands.pop();
}
