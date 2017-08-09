#include "main_menu_state.hpp"
#include "libtcod.hpp"

void MainMenuState::init() {
	MenuItem newGame = { MenuItemCode::NEW_GAME, "New game!" };
	MenuItem exit = { MenuItemCode::EXIT, "Exit!" };
	menuItems.push_back(newGame);
	menuItems.push_back(exit);
}

void MainMenuState::cleanup() {

}

void MainMenuState::update(Engine* engine) {
	showMenu(engine);
}

void MainMenuState::render(Engine* engine) {
}

void MainMenuState::showMenu(Engine* engine) {
	int menuX = 10;
	int menuY = constants::SCREEN_HEIGHT / 3;
	console.setDefaultBackground(TCODColor::black);
	console.setDefaultForeground(TCODColor::white);
	console.print(1, 1, "AAAAA");

	int currentItem = 0;
	for(MenuItem item : menuItems) {
		console.print(menuX, menuY + currentItem * 3, item.label.c_str());
		++currentItem;
	}

	console.flush();
	TCODConsole::blit(&console, 0, 0, constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT, TCODConsole::root, 0, 0);
}
