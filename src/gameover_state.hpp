#ifndef GAME_OVER_STATE_HPP
#define GAME_OVER_STATE_HPP
#include "constants.hpp"
#include "state.hpp"
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

class Engine;
class Actor;

class GameOverState : public State {
public:
	GameOverState(Engine* engine, Actor* actor);

	void handleEvents() override;
	void update(sf::RenderWindow& window) override;
	void render(sf::RenderWindow& window) override;

private:
	Actor* actor;
	std::string description;
};
#endif /* GAME_OVER_STATE_HPP */
