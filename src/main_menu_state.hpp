#ifndef MAIN_MENU_STATE_HPP
#define MAIN_MENU_STATE_HPP
#include "state.hpp"
#include "constants.hpp"
#include <iostream>
#include <vector>
#include "libtcod.hpp"

class Engine;
class MainMenuState : public State {
public:
	enum class MenuItemCode { NONE, NEW_GAME, CONTINUE, EXIT };

	MainMenuState() : console(constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT) {;}
	void init() override;
	void cleanup() override;

	void handleEvents(Engine* engine) override;
	void update(Engine* engine) override;
	void render(Engine* engine) override;
private:
	struct MenuItem {
		MenuItemCode code;
		std::string label;
	};

	std::vector<MenuItem> menuItems;
	int selectedItem;

	TCODConsole console;
	void showMenu(Engine* engine);
};
#endif /* MAIN_MENU_STATE_HPP */
