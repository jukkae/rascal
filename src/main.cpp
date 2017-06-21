#include "main.hpp"

std::string file = "save.txt";

Engine engine(80, 50);

int main() {
	showMenu();

	while(!TCODConsole::isWindowClosed()) {
		TCOD_key_t lastKey;
		TCOD_mouse_t mouse;
		TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE, &lastKey, &mouse, true);
		if(lastKey.vk == TCODK_ESCAPE) {
			save();
			load();
			showMenu();
		} else {
			engine.lastKey = lastKey;
			engine.mouse = mouse;
		}
		engine.update();
		engine.render();
		TCODConsole::flush();
	}

	save();
	return 0;
}

void load() {
	std::ifstream ifs(file);
	boost::archive::text_iarchive ia(ifs);
	ia >> engine;
}

void save() {
	std::ofstream ofs(file);
	boost::archive::text_oarchive oa(ofs);
	oa << engine;
}

void showMenu() {
	engine.gui.menu.clear();
	engine.gui.menu.addItem(Menu::MenuItemCode::NEW_GAME, "New game");
	if(TCODSystem::fileExists(file.c_str())) {
		engine.gui.menu.addItem(Menu::MenuItemCode::CONTINUE, "Continue");
	}
	engine.gui.menu.addItem(Menu::MenuItemCode::EXIT, "Exit");
	Menu::MenuItemCode menuItem = engine.gui.menu.pick();

	if (menuItem == Menu::MenuItemCode::EXIT || menuItem == Menu::MenuItemCode::NONE) {
		exit(0);
	} else if (menuItem == Menu::MenuItemCode::NEW_GAME) {
		engine.term();
		engine.init();
	} else {
		engine.term();
		load();
		engine.gameStatus = Engine::GameStatus::STARTUP;
	}
}
