#ifndef VICTORY_STATE_HPP
#define VICTORY_STATE_HPP
#include "state.hpp"
#include <string>
#include <SFML/Graphics.hpp>

class Engine;
class VictoryState : public State {
public:
	VictoryState(Engine* engine, Actor* actor);

	void handleEvents() override;
	void update(sf::RenderWindow& window) override;
	void render(sf::RenderWindow& window) override;

private:
	Actor* actor;
	std::string description;
};
#endif /* VICTORY_STATE_HPP */
