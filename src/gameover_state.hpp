#ifndef GAME_OVER_STATE
#define GAME_OVER_STATE
#include "constants.hpp"
#include "state.hpp"
#include <string>
#include <vector>
//#include "libtcod.hpp"
#include <SFML/Graphics.hpp>

class Engine;
class Actor;

class GameOverState : public State {
public:
	GameOverState(Actor* actor);

	void init(Engine* engine) override;
	void cleanup() override;

	void handleEvents(Engine* engine) override;
	void update(Engine* engine, sf::RenderWindow& window) override;
	void render(Engine* engine, sf::RenderWindow& window) override;

private:
	//TCODConsole console;
	sf::Font font;
	Actor* actor;
	std::string description;
};
#endif /* GAME_OVER_STATE */
