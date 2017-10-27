#ifndef ENGINE_HPP
#define ENGINE_HPP
class Actor;
class State;
#include <queue>
#include <string>

#include "renderer.hpp"
#include "gui.hpp"
#include "attacker.hpp"
#include "engine_command.hpp"
#include "persistent.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class Engine {
public:
	Engine();
	~Engine();
	void init();
	void newGame();

	void pushState(State* state) { states.push_back(state); }
	void popState() { states.pop_back(); }
	void update(sf::RenderWindow& window);

	template <class T>
	void addEngineCommand(T engineCommand) { engineCommands.push(std::make_unique<T>(engineCommand)); }

	void addEvent(sf::Event event) { events.push(event); }
	bool pollEvent(sf::Event& event);

	void exit();
	void save();
	void load();
	void changeState(State* state) { popState(); pushState(state); }

private:
	State* gameplayState;
	std::vector<State*> states;
	std::queue<std::unique_ptr<EngineCommand>> engineCommands;
	std::queue<sf::Event> events;

	void executeEngineCommand();
};
#endif /* ENGINE_HPP */
