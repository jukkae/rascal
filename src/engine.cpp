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
	while(engineCommands.size() > 0) executeEngineCommand();

	states.back()->handleEvents(this);
	states.back()->update(this);
	states.back()->render(this);

	TCODConsole::root->flush();
}

void Engine::executeEngineCommand() {
	engineCommands.front()->execute();
	engineCommands.pop();
}

void Engine::exit() {
	// TODO serialize
	::exit(0);
}

/*
std::string file = "save.txt";
void load() {
	std::ifstream ifs(file);
	boost::archive::text_iarchive ia(ifs);
	ia >> engine;
}

void save() {
	std::ofstream ofs(file);
	boost::archive::text_oarchive oa(ofs);
	oa << engine; // no need to serialize this, just gameplaystate and gui (log)
}
*/
