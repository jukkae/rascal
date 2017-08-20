#include "main_menu_state.hpp"
#include "engine.hpp"
#include "libtcod.hpp"

void MainMenuState::init() {
	console.setDefaultBackground(TCODColor::black);

	MenuItem newGame = { MenuItemCode::NEW_GAME, "New game!" };
	MenuItem cont = { MenuItemCode::CONTINUE, "Continue!" };
	MenuItem exit = { MenuItemCode::EXIT, "Exit!" };
	MenuItem dummy = { MenuItemCode::NONE, "This is just to throw you off" };
	menuItems.push_back(newGame);
	menuItems.push_back(cont);
	menuItems.push_back(exit);
	menuItems.push_back(dummy);

	selectedItem = 0;
}

void MainMenuState::cleanup() {

}

void MainMenuState::handleEvents(Engine* engine) {
	TCOD_key_t key;
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, nullptr, false); // blocks, nasty, but will work for now

	switch (key.vk) { // TODO don't do this like this mmkay
		case TCODK_UP:
			if(selectedItem > 0) --selectedItem;
			break;
		case TCODK_DOWN:
			if(selectedItem < menuItems.size() - 1) ++selectedItem;
			break;
		case TCODK_ENTER:
			handleSelectedMenuItem(engine);
			break;
		default:
			break;
	}
	showMenu(engine);
}

void MainMenuState::update(Engine* engine) {

}

void MainMenuState::render(Engine* engine) {
	showMenu(engine);
}

void MainMenuState::showMenu(Engine* engine) {
	int menuX = 10;
	int menuY = constants::SCREEN_HEIGHT / 3;

	int itemIndex = 0;
	for(MenuItem item : menuItems) {
		if (selectedItem == itemIndex) {
			console.setDefaultForeground(TCODColor::lighterOrange);
		} else {
			console.setDefaultForeground(TCODColor::lightGrey);
		}

		console.print(menuX, menuY + itemIndex * 3, item.label.c_str());
		++itemIndex;
	}

	console.flush();
	TCODConsole::blit(&console, 0, 0, constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT, TCODConsole::root, 0, 0);
}

void MainMenuState::handleSelectedMenuItem(Engine* engine) {
	// TODO rather send a command to engine
	switch (menuItems.at(selectedItem).code) {
		case MenuItemCode::NEW_GAME:
			break;
		case MenuItemCode::CONTINUE:
			engine->addCommand(new DummyCommand(engine));
			break;
		case MenuItemCode::EXIT:
			break;
		case MenuItemCode::NONE:
			break;
		default:
			break;
	}
}
