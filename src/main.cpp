#include "main.hpp"

std::string file = "save.txt";

Engine engine(80, 50);

int main() {
	// Show menu on startup
	engine.gui->menu.clear();
	engine.gui->menu.addItem(Menu::NEW_GAME, "New game");
	if(TCODSystem::fileExists(file.c_str())) {
		engine.gui->menu.addItem(Menu::CONTINUE, "Continue");
	}
	engine.gui->menu.addItem(Menu::EXIT, "Exit");
	Menu::MenuItemCode menuItem = engine.gui->menu.pick();

	if (menuItem == Menu::EXIT || menuItem == Menu::NONE) {
		exit(0);
	} else if (menuItem == Menu::NEW_GAME) {
		engine.term();
		engine.init();
	} else {
		engine.term();
		std::ifstream ifs(file);
		boost::archive::text_iarchive ia(ifs);
		ia >> engine;
		engine.gameStatus = Engine::STARTUP;
	}

	// Main game loop
	while(!TCODConsole::isWindowClosed()) {
		// TODO open game menu by pressing esc
		// TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE, &lastKey, &mouse);
		// if(lastKey.vk == TCODK_ESCAPE)
		// { save & load engine and reshow menu }
		engine.update();
		engine.render();
		TCODConsole::flush();
	}

	// TODO implement permadeath
	// Save starts
	std::ofstream ofs(file);
	boost::archive::text_oarchive oa(ofs);
	oa << engine;   
	// Save ends

	return 0;
}
