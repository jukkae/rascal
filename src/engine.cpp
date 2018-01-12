#include <algorithm>
#include <limits>
#include <cstdio>
#include "actor.hpp"
#include "ai.hpp"
#include "constants.hpp"
#include "container.hpp"
#include "destructible.hpp"
#include "effect.hpp"
#include "engine.hpp"
#include "engine_command.hpp"
#include "font.hpp"
#include "io.hpp"
#include "pickable.hpp"
#include "state.hpp"
#include "gameplay_state.hpp"
#include "main_menu_state.hpp"
#include "status_effect.hpp"
#include "world.hpp"


Engine::Engine(sf::RenderWindow* window) : window(window) {
	font::load();
	std::unique_ptr<State> gps = std::make_unique<GameplayState>(this, window);

	bool showContinueInMenu = false;
	if(!io::fileExists(constants::SAVE_FILE_NAME)) {
		gameplayState = gps.get();
	}
	else { // FIXME urgh
		load();
		(static_cast<GameplayState&>(*gameplayState)).initLoaded(this);
		showContinueInMenu = true;
		gps = std::unique_ptr<GameplayState>(static_cast<GameplayState*>(gameplayState));
	}

	std::unique_ptr<State> mainMenuState = std::make_unique<MainMenuState>(this, showContinueInMenu);

	states.push_back(std::move(gps));
	states.push_back(std::move(mainMenuState));
}

Engine::~Engine() {
}

void Engine::newGame() {
	if(io::fileExists(constants::SAVE_FILE_NAME)) {
		io::removeFile(constants::SAVE_FILE_NAME);
	}

	std::unique_ptr<State> gps = std::make_unique<GameplayState>(this, window);
	gameplayState = gps.get();
	states.at(0) = std::move(gps);
}

void Engine::update() {
	while(engineCommands.size() > 0) executeEngineCommand();
	states.back()->update();
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
	engineCommands.push(std::make_unique<ChangeStateCommand>(this, std::move(state)));
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
	ia.register_type<StatusEffectEffect>();
	ia.register_type<StatusEffectRemovalEffect>();
	ia.register_type<MoveEffect>();

	ia.register_type<PoisonedStatusEffect>();

	ia.template register_type<EffectGeneratorFor<MoveEffect>>();

	ia >> gameplayState;
	(static_cast<GameplayState*>(gameplayState))->setEngine(this);
	(static_cast<GameplayState*>(gameplayState))->setWindow(window);
}

void Engine::save() {
	std::ofstream ofs(constants::SAVE_FILE_NAME);
	boost::archive::text_oarchive oa(ofs);

	oa.register_type<HealthEffect>();
	oa.register_type<AiChangeEffect>();
	oa.register_type<StatusEffectEffect>();
	oa.register_type<StatusEffectRemovalEffect>();
	oa.register_type<MoveEffect>();

	oa.register_type<PoisonedStatusEffect>();

	oa.template register_type<EffectGeneratorFor<MoveEffect>>();

	oa << gameplayState;
}
