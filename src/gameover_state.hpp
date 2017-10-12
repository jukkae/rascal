#ifndef GAME_OVER_STATE
#define GAME_OVER_STATE
#include "constants.hpp"
#include "state.hpp"
#include <string>
#include <vector>
#include "libtcod.hpp"

class Engine;
class Actor;

class GameOverState : public State {
public:
	GameOverState(Actor* actor) : console(constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT), actor(actor) {;}

	void init(Engine* engine) override;
	void cleanup() override;

	void handleEvents(Engine* engine) override;
	void update(Engine* engine) override;
	void render(Engine* engine) override;

private:
	TCODConsole console;
	Actor* actor;
};
#endif /* GAME_OVER_STATE */
