#ifndef MAIN_MENU_STATE_HPP
#define MAIN_MENU_STATE_HPP
#include "state.hpp"
#include "constants.hpp"
#include <iostream>
#include "libtcod.hpp"

class Engine;
class MainMenuState : public State {
public:
	MainMenuState() : console(constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT) {;}
	void init() override;
	void cleanup() override;

	void update(Engine* engine) override;
	void render(Engine* engine) override;
private:
	TCODConsole console;
	void showMenu(Engine* engine);
};
#endif /* MAIN_MENU_STATE_HPP */
