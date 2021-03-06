#ifndef ENGINE_HPP
#define ENGINE_HPP
class State;
#include "preferences.hpp"
#include <queue>

#include <SFML/Window/Event.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "audio.hpp"
#include "lisp/lisp.hpp"
class EngineCommand;
class Engine {
public:
	Engine(sf::RenderWindow* window);
	~Engine();
	void newGame();

	void pushState(std::unique_ptr<State> state);
	void changeState(std::unique_ptr<State> state);
	void popState();

	void update();

	template <class T>
	void addEngineCommand(T engineCommand) { engineCommands.push(std::make_unique<T>(engineCommand)); }

	void addEvent(sf::Event event) { events.push(event); }
	bool pollEvent(sf::Event& event);

	void exit();
	void save();
	void load();

	sf::RenderWindow* getWindow() { return window; }
	bool gameOver = false; //TODO this is bad and i feel bad
	Preferences preferences;
	AudioPlayer audioPlayer; //FIXME let's have this public for now
	lisp::Atom env; // LISP execution environment
	void initializeLisp();
	void lispRepl();
private:
	sf::RenderWindow* window;
	State* gameplayState;
	std::vector<std::unique_ptr<State>> states;
	std::queue<std::unique_ptr<EngineCommand>> engineCommands;
	std::queue<sf::Event> events;

	void executeEngineCommand();
	void loadPreferences();
	void savePreferences();
};
#endif /* ENGINE_HPP */
