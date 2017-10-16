#include "main_menu_state.hpp"
#include "constants.hpp"
#include "engine.hpp"
#include "libtcod.hpp"

void MainMenuState::init(Engine* engine) {
	console.setDefaultBackground(TCODColor::black);

	MenuItem newGame = { MenuItemCode::NEW_GAME, "New game!" };
	MenuItem cont = { MenuItemCode::CONTINUE, "Continue!" };
	MenuItem exit = { MenuItemCode::EXIT, "Exit!" };
	menuItems.push_back(newGame);
	//if(TCODSystem::fileExists(constants::SAVE_FILE_NAME.c_str())) {
		menuItems.push_back(cont);
	//}
	menuItems.push_back(exit);

	selectedItem = 0;
}

void MainMenuState::cleanup() {

}

void MainMenuState::handleEvents(Engine* engine) {
	TCOD_key_t key;
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &key, nullptr);

	switch (key.vk) {
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
	switch (menuItems.at(selectedItem).code) {
		case MenuItemCode::NEW_GAME:
			engine->addEngineCommand(NewGameCommand(engine));
			break;
		case MenuItemCode::CONTINUE:
			engine->addEngineCommand(ContinueCommand(engine));
			break;
		case MenuItemCode::EXIT:
			engine->addEngineCommand(ExitCommand(engine));
			break;
		case MenuItemCode::NONE:
			break;
		default:
			break;
	}
}
