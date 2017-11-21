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

	MainMenuState(bool forceShowContinue = false) : forceShowContinue(forceShowContinue) {;}
	void init(Engine* engine) override;
	void cleanup() override;

	void handleEvents(Engine* engine) override;
	void update(Engine* engine, sf::RenderWindow& window) override;
	void render(Engine* engine, sf::RenderWindow& window) override;
private:
	sf::Font font;
	bool forceShowContinue;

	struct MenuItem {
		MenuItemCode code;
		std::string label;
	};

	std::vector<MenuItem> menuItems;
	int selectedItem;

	void handleSelectedMenuItem(Engine* engine);
	void showMenu(Engine* engine, sf::RenderWindow& window);
};
#endif /* MAIN_MENU_STATE_HPP */
