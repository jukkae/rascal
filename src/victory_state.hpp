#ifndef VICTORY_STATE_HPP
#define VICTORY_STATE_HPP
#include "state.hpp"
#include <string>
#include <SFML/Graphics.hpp>

class Engine;
class VictoryState : public State {
public:
	VictoryState(Actor* actor);

	void init(Engine* engine) override;
	void cleanup() override;

	void handleEvents(Engine* engine) override;
	void update(Engine* engine, sf::RenderWindow& window) override;
	void render(Engine* engine, sf::RenderWindow& window) override;

private:
	Actor* actor;
	std::string description;
};
#endif /* VICTORY_STATE_HPP */
