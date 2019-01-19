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

#include "lisp/lisp.hpp"

Engine::Engine(sf::RenderWindow* window) : window(window) {
	font::load();
	loadPreferences();
	if(preferences.music.second) audioPlayer.music.play();
	else audioPlayer.music.stop();

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

	// LISP TESTS
	lisp::Atom a = lisp::makeInt(42);
	lisp::printExpr(a);
  std::cout << "\n";

	lisp::Atom hello = lisp::makeSymbol("hello from lisp!");
	lisp::printExpr(hello);
  std::cout << "\n";

	lisp::Atom nil = lisp::makeNil();
	lisp::printExpr(nil);
  std::cout << "\n";

	lisp::Atom pair = lisp::cons(lisp::makeSymbol("head"), lisp::makeSymbol("tail"));
	lisp::printExpr(pair);
	std::cout << "\n";

	lisp::Atom list = lisp::cons(lisp::makeInt(1),
										lisp::cons(lisp::makeInt(2),
										lisp::cons(lisp::makeInt(3),
									 	lisp::makeNil())));
	lisp::printExpr(list);
	std::cout << "\n";

	lisp::Atom h2 = lisp::makeSymbol("hello from lisp!");
	lisp::printExpr(h2);
	std::cout << "\n";

	std::cout << "symbol table:\n";
	lisp::printExpr(lisp::symbolTable);
	std::cout << "\n";

	std::string toBeTokenized = "(foo bar)";
	std::cout << "tokenizing \'" << toBeTokenized << "\':\n";
	std::vector<std::string> tokens = lisp::tokenize(toBeTokenized);
	for(auto& t : tokens) {
		std::cout << t << "\n";
	}
}

Engine::~Engine() {
}

void Engine::newGame() {
	gameOver = false;
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
	savePreferences();
	if(!gameOver) save();
	//::exit(0);
	io::window.close();
}

void Engine::load() {
	gameOver = false;
	std::ifstream ifs(constants::SAVE_FILE_NAME);
	boost::archive::text_iarchive ia(ifs);

	ia.register_type<HealthEffect>();
	ia.register_type<AiChangeEffect>();
	ia.register_type<StatusEffectEffect>();
	ia.register_type<StatusEffectRemovalEffect>();
	ia.register_type<MoveEffect>();

	ia.register_type<PoisonedStatusEffect>();
	ia.register_type<AttributeModifierStatusEffect>();

	ia.template register_type<EffectGeneratorFor<MoveEffect>>();
	ia.template register_type<EffectGeneratorFor<StatusEffectEffect>>();

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
	oa.register_type<AttributeModifierStatusEffect>();

	oa.template register_type<EffectGeneratorFor<MoveEffect>>();
	oa.template register_type<EffectGeneratorFor<StatusEffectEffect>>();

	oa << gameplayState;
}

void Engine::loadPreferences() {
	std::ifstream fin("assets/preferences.txt");
	fin >> preferences;
}

void Engine::savePreferences() {
	std::ofstream fout("assets/preferences.txt");
	fout << preferences;
}
