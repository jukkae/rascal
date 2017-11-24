#ifndef MAIN_MENU_STATE_HPP
#define MAIN_MENU_STATE_HPP
#include "state.hpp"
#include "constants.hpp"
#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>

class Engine;
class MainMenuState : public State {
public:
	enum class MenuItemCode { NONE, NEW_GAME, CONTINUE, EXIT };

	MainMenuState(Engine* engine, bool forceShowContinue = false);

	void handleEvents() override;
	void update() override;
	void render() override;
private:
	sf::Font font;
	bool forceShowContinue;

	struct MenuItem {
		MenuItemCode code;
		std::string label;
	};

	std::vector<MenuItem> menuItems;
	int selectedItem;
	std::string asciiTitle;

	void handleSelectedMenuItem();
	void renderAsciiTitle();
	void showMenu();
};
#endif /* MAIN_MENU_STATE_HPP */
