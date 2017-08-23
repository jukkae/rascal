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

private:
	DummyState dummyState;
	GameplayState gameplayState;
	MainMenuState mainMenuState;
	std::vector<State*> states;
	std::queue<EngineCommand*> engineCommands;

	void executeEngineCommand();

	friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		ar & gameplayState; // TODO of course, serialize states
    }
};

extern Engine engine; // TODO get rid of this, implement in a more robust way
#endif /* ENGINE_HPP */
