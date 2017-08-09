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
	int selectedItem = 0;
	for(MenuItem item : menuItems) {
		if (currentItem == selectedItem) {
			console.setDefaultForeground(TCODColor::lighterOrange);
		} else {
			console.setDefaultForeground(TCODColor::lightGrey);
		}

		console.print(menuX, menuY + currentItem * 3, item.label.c_str());
		++currentItem;
	}

	TCOD_key_t key;
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, nullptr, true); // TODO how to pass events from engine to here?
	switch (key.vk) {
		case TCODK_UP:
			--selectedItem;
			if (selectedItem < 0) {
				selectedItem = menuItems.size() - 1;
			}
			break;
		case TCODK_DOWN:
			selectedItem = (selectedItem + 1) % menuItems.size();
			break;
		case TCODK_ENTER:
			//return items.at(selectedItem).code;
		default:
			break;
	}

	console.flush();
	TCODConsole::blit(&console, 0, 0, constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT, TCODConsole::root, 0, 0);
}
