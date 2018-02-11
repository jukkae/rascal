#ifndef GAME_OVER_STATE_HPP
#define GAME_OVER_STATE_HPP
#include "state.hpp"
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

class Engine;
class Actor;
class Player;

class GameOverState : public State {
public:
	GameOverState(Engine* engine, Actor* actor, Player* player, bool victory = false);

	void handleEvents() override;
	void update() override;
	void render() override;

private:
	std::string description;
};
#endif /* GAME_OVER_STATE_HPP */
