#include <algorithm>
#include <limits>
#include <sys/stat.h>
#include <stdio.h>
#include "ai.hpp"
#include "constants.hpp"
#include "container.hpp"
#include "destructible.hpp"
#include "engine.hpp"
#include "font.hpp"
#include "pickable.hpp"
#include "state.hpp"
#include "gameplay_state.hpp"
#include "main_menu_state.hpp"


Engine::Engine() {
	font::initialize();
	init();
}

Engine::~Engine() {
}

void Engine::init() {
	std::unique_ptr<State> gps = std::make_unique<GameplayState>();

	struct stat buf;
	int result = stat (constants::SAVE_FILE_NAME.c_str(), &buf);
	std::cout << "engine: " << result << "\n";

	bool showContinueInMenu = false;
	if(result != 0) { // If file doesn't exist
		gps->init(this);
	}
	else {
		load();
		(static_cast<GameplayState&>(*gps)).initLoaded(this);
		showContinueInMenu = true;
	}
	gameplayState = gps.get();

	std::unique_ptr<State> mainMenuState = std::make_unique<MainMenuState>(showContinueInMenu);
	mainMenuState->init(this);

	states.push_back(std::move(gps));
	states.push_back(std::move(mainMenuState));
}

void Engine::newGame() {
	struct stat buffer;
	if(stat (constants::SAVE_FILE_NAME.c_str(), &buffer) == 0) { // If file does exist
		remove(constants::SAVE_FILE_NAME.c_str());
	}

	std::unique_ptr<State> gps = std::make_unique<GameplayState>();
	gps->init(this);
	gameplayState = gps.get();
	states.at(0) = std::move(gps);
}

void Engine::update(sf::RenderWindow& window) {

	while(engineCommands.size() > 0) executeEngineCommand();
	states.back()->update(this, window);
}

bool Engine::pollEvent(sf::Event& event) {
	if (events.empty()) return false;
	else {
		event = events.front();
		events.pop();
		return true;
	}

}

void Engine::pushState(std::unique_ptr<State> state) {
	states.push_back(std::move(state));
}


void Engine::changeState(std::unique_ptr<State> state) {
	popState();
	pushState(std::move(state));
}

void Engine::popState() {
	states.pop_back();
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
	std::ifstream ifs(constants::SAVE_FILE_NAME);
	boost::archive::text_iarchive ia(ifs);

	ia.register_type<HealthEffect>();
	ia.register_type<AiChangeEffect>();

	ia >> gameplayState;

}

void Engine::save() {
	std::ofstream ofs(constants::SAVE_FILE_NAME);
	boost::archive::text_oarchive oa(ofs);

	oa.register_type<HealthEffect>();
	oa.register_type<AiChangeEffect>();

	oa << gameplayState;
}
