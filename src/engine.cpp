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
	std::cout << "Testing LISP subsystem\n";
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

	std::string toBeTokenized = "(foo bar(123 12 . -3) nil)";
	std::cout << "tokenizing \'" << toBeTokenized << "\':\n";
	std::list<std::string> tokens = lisp::tokenize(toBeTokenized);
	for(auto& t : tokens) {
		std::cout << t << ",";
	}
	std::cout << "\n";

	std::string toBeParsed = "(foo (bar . quux) xyzzy . (woof . nil))";
	{
		std::string p = "nil";
		std::cout << "parsing \'" << p << "\':\n";
		lisp::Atom a = lisp::readExpression(p);
		lisp::printExpr(a);
		std::cout << "\n";
	}

	{
		std::string p = "123";
		std::cout << "parsing \'" << p << "\':\n";
		lisp::Atom a = lisp::readExpression(p);
		lisp::printExpr(a);
		std::cout << "\n";
	}

	{
		std::string p = "bork";
		std::cout << "parsing \'" << p << "\':\n";
		lisp::Atom a = lisp::readExpression(p);
		lisp::printExpr(a);
		std::cout << "\n";
	}

	{
		std::string p = "(a b c d)";
		std::cout << "parsing \'" << p << "\':\n";
		lisp::Atom a = lisp::readExpression(p);
		lisp::printExpr(a);
		std::cout << "\n";
	}

	{
		std::string p = "()";
		std::cout << "parsing \'" << p << "\':\n";
		lisp::Atom a = lisp::readExpression(p);
		lisp::printExpr(a);
		std::cout << "\n";
	}

	{
		std::string p1 = "(foo (bar (quux (bork boof))))";
		std::cout << "parsing \'" << p1 << "\':\n";
		lisp::Atom a1 = lisp::readExpression(p1);
		lisp::printExpr(a1);
		std::cout << "\n";
	}

	{
		std::string p2 = "(foo (bar (quux nil)))";
		std::cout << "parsing \'" << p2 << "\':\n";
		lisp::Atom a2 = lisp::readExpression(p2);
		lisp::printExpr(a2);
		std::cout << "\n";
	}

	{
		std::string p = "()";
		std::cout << "parsing \'" << p << "\':\n";
		lisp::Atom a = lisp::readExpression(p);
		lisp::printExpr(a);
		std::cout << "\n";
	}

	{
		std::string p = "(bork (boof))";
		std::cout << "parsing \'" << p << "\':\n";
		lisp::Atom a = lisp::readExpression(p);
		lisp::printExpr(a);
		std::cout << "\n";
	}

	{
		std::string p = "(a b c d)";
		std::cout << "parsing \'" << p << "\':\n";
		lisp::Atom a = lisp::readExpression(p);
		lisp::printExpr(a);
		std::cout << "\n";
	}

	{
		std::string p = "(a b c . d)";
		std::cout << "parsing \'" << p << "\':\n";
		lisp::Atom a = lisp::readExpression(p);
		lisp::printExpr(a);
		std::cout << "\n";
	}

	{
		std::string p = "((a b c d nil))";
		std::cout << "parsing \'" << p << "\':\n";
		lisp::Atom a = lisp::readExpression(p);
		lisp::printExpr(a);
		std::cout << "\n";
	}

	{
		std::string p = "((a b c d))";
		std::cout << "parsing \'" << p << "\':\n";
		lisp::Atom a = lisp::readExpression(p);
		lisp::printExpr(a);
		std::cout << "\n";
	}

	{
		std::string p = "((a b) (a c))";
		std::cout << "parsing \'" << p << "\':\n";
		lisp::Atom a = lisp::readExpression(p);
		lisp::printExpr(a);
		std::cout << "\n";
	}

	{
		std::string p = "((a b) d (a c) b d nil)";
		std::cout << "parsing \'" << p << "\':\n";
		lisp::Atom a = lisp::readExpression(p);
		lisp::printExpr(a);
		std::cout << "\n";
	}

	{
		std::string p = "(()((u cant) break (my parser nil nil) with (anything nil) at all))";
		std::cout << "parsing \'" << p << "\':\n";
		lisp::Atom a = lisp::readExpression(p);
		lisp::printExpr(a);
		std::cout << "\n";
	}

	{
		std::string p = "(a . b)";
		std::cout << "parsing \'" << p << "\':\n";
		lisp::Atom a = lisp::readExpression(p);
		lisp::printExpr(a);
		std::cout << "\n";
	}

	{
		std::string p = "a b nil";
		std::cout << "parsing \'" << p << "\':\n";
		lisp::Atom a = lisp::readExpression(p);
		lisp::printExpr(a);
		std::cout << "\n";
	}

	{
		std::string p = "(s (t . u) v . (w . nil))";
		std::cout << "parsing \'" << p << "\':\n";
		lisp::Atom a = lisp::readExpression(p);
		lisp::printExpr(a);
		std::cout << "\n";
	}

	std::cout << "\n\n";
	//::exit(0);

	{
		std::cout << "Testing env and expressions\n";
		std::cout << "Creating env\n";
		lisp::Atom parent = lisp::makeNil();
		lisp::Atom env = lisp::createEnv(parent);
		std::cout << "env:\n";
		lisp::printExpr(env);
		std::cout << "\n";

		std::cout << "Creating symbol and value and setting value\n";
		lisp::Atom symbol = lisp::makeSymbol("foo");
		lisp::Atom value = lisp::makeInt(3);
		lisp::setEnv(env, symbol, value);
		std::cout << "env:\n";
		lisp::printExpr(env);
		std::cout << "\n";

		std::cout << "Getting value\n";
		lisp::Atom result = lisp::getEnv(env, symbol);
		std::cout << "result:\n";
		lisp::printExpr(result);
		std::cout << "\n";
	}
	std::cout << "\n\n";
	// REPL
	lisp::Atom env = lisp::createEnv(lisp::makeNil());
	lisp::setEnv(env, lisp::makeSymbol("head"), lisp::makeBuiltin(lisp::Builtin{lisp::builtinHead}));
	lisp::setEnv(env, lisp::makeSymbol("tail"), lisp::makeBuiltin(lisp::Builtin{lisp::builtinTail}));
	lisp::setEnv(env, lisp::makeSymbol("cons"), lisp::makeBuiltin(lisp::Builtin{lisp::builtinCons}));
	lisp::setEnv(env, lisp::makeSymbol("+"), lisp::makeBuiltin(lisp::Builtin{lisp::builtinAdd}));
	lisp::setEnv(env, lisp::makeSymbol("-"), lisp::makeBuiltin(lisp::Builtin{lisp::builtinSubtract}));
	lisp::setEnv(env, lisp::makeSymbol("*"), lisp::makeBuiltin(lisp::Builtin{lisp::builtinMultiply}));
	lisp::setEnv(env, lisp::makeSymbol("/"), lisp::makeBuiltin(lisp::Builtin{lisp::builtinDivide}));
	lisp::setEnv(env, lisp::makeSymbol("%"), lisp::makeBuiltin(lisp::Builtin{lisp::builtinModulo}));

	lisp::setEnv(env, lisp::makeSymbol("t"), lisp::makeSymbol("t"));

	for(;;) {
		std::cout << "lisp> ";
		std::string s;
		getline(std::cin, s);
		try {
			lisp::Atom expression = lisp::readExpression(s);
			lisp::Atom result = lisp::evaluateExpression(expression, env);
			lisp::printExpr(result);
		}
		catch(lisp::LispException e) {
			std::cout << "LISP runtime exception: " << e.what();
		}

		std::cout << "\n";
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
