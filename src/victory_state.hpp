#ifndef VICTORY_STATE_HPP
#define VICTORY_STATE_HPP
#include "state.hpp"
#include <string>
#include <SFML/Graphics.hpp>

class Engine;
class Actor;
class VictoryState : public State {
public:
	VictoryState(Engine* engine, Actor* actor);

	void handleEvents() override;
	void update() override;
	void render() override;

private:
	std::string description;
};
#endif /* VICTORY_STATE_HPP */
