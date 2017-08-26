#include <algorithm>
#include <limits>
#include "libtcod.hpp"
#include "ai.hpp"
#include "constants.hpp"
#include "container.hpp"
#include "destructible.hpp"
#include "engine.hpp"

std::string file = "save.txt";

Engine::Engine() {
	TCODConsole::initRoot(constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT, "Rascal", false);
	init();
}

Engine::~Engine() {
}

void Engine::init() {
	gameplayState = new GameplayState(); // TODO yes leaks memory
	gameplayState->init();
	State* mainMenuState = new MainMenuState();
	mainMenuState->init();

	states.push_back(gameplayState);
	states.push_back(mainMenuState);
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
	save();
	::exit(0);
}

void Engine::load() {
	std::ifstream ifs(file);
	boost::archive::text_iarchive ia(ifs);
	ia >> gameplayState;
}

void Engine::save() {
	std::ofstream ofs(file);
	boost::archive::text_oarchive oa(ofs);
	oa << gameplayState;
}
