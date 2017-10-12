#ifndef ENGINE_HPP
#define ENGINE_HPP
class Actor;
class State;
#include <queue>

#include "renderer.hpp"
#include "gui.hpp"
#include "attacker.hpp"
#include "engine_command.hpp"
#include "persistent.hpp"

class Engine {
public:
	Engine();
	~Engine();
	void init();

	void pushState(State* state) { states.push_back(state); }
	void popState() { states.pop_back(); }
	void update();
	void addEngineCommand(EngineCommand* engineCommand) { engineCommands.push(engineCommand); }
	void exit();
	void save();
	void load();
	void changeState(State* state) { popState(); pushState(state); }

private:
	State* gameplayState;
	std::vector<State*> states;
	std::queue<EngineCommand*> engineCommands;

	void executeEngineCommand();
};
#endif /* ENGINE_HPP */
