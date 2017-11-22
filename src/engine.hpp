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
	void newGame();

	void pushState(std::unique_ptr<State> state);
	void changeState(std::unique_ptr<State> state);
	void popState();

	void update(sf::RenderWindow& window);

	template <class T>
	void addEngineCommand(T engineCommand) { engineCommands.push(std::make_unique<T>(engineCommand)); }

	void addEvent(sf::Event event) { events.push(event); }
	bool pollEvent(sf::Event& event);

	void exit();
	void save();
	void load();

private:
	State* gameplayState;
	std::vector<std::unique_ptr<State>> states;
	std::queue<std::unique_ptr<EngineCommand>> engineCommands;
	std::queue<sf::Event> events;

	void executeEngineCommand();
};
#endif /* ENGINE_HPP */
