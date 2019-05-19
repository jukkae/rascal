#ifndef MAIN_MENU_STATE_HPP
#define MAIN_MENU_STATE_HPP
#include "state.hpp"
#include <vector>
#include <SFML/Graphics.hpp>

class Engine;
class MainMenuState : public State {
public:
	enum class MenuItemCode { NONE, NEW_GAME, CONTINUE, HELP, PREFERENCES, EXIT };

	MainMenuState(Engine* engine, bool forceShowContinue = false);

	void handleEvents() override;
	void update() override;
	void render() override;
private:
	sf::Font font;

	struct MenuItem {
		MenuItemCode code;
		std::string label;
	};

	std::vector<MenuItem> menuItems;
	int selectedItem;
	std::string asciiTitle;
	std::string bgArt;

	void handleSelectedMenuItem();
	void renderAsciiTitle();
	void renderBgArt();
	void showMenu();
};
#endif /* MAIN_MENU_STATE_HPP */
