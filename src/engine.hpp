#ifndef ENGINE_HPP
#define ENGINE_HPP
class Actor;
#include <queue>

#include "renderer.hpp"
#include "gui.hpp"
#include "attacker.hpp"
#include "engine_command.hpp"
#include "state.hpp"
#include "dummy_state.hpp"
#include "gameplay_state.hpp"
#include "main_menu_state.hpp"
#include "persistent.hpp"

class Engine {
public:
	TCOD_key_t lastKey;
	TCOD_mouse_t mouse;

	Engine(int dummy); // dummy parameter due to idiotic extern
	~Engine();
	void init();

	//void changeState(State* state);
	void pushState(State* state) { states.push_back(state); }
	void popState() { states.pop_back(); }
	void update();
	void addEngineCommand(EngineCommand* engineCommand) { engineCommands.push(engineCommand); }
	void exit();
	void save();
	void load();

private:
	DummyState dummyState;
	GameplayState gameplayState;
	MainMenuState mainMenuState;
	std::vector<State*> states;
	std::queue<EngineCommand*> engineCommands;

	void executeEngineCommand();
};
#endif /* ENGINE_HPP */
