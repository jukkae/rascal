#include "main.hpp"

std::string file = "save.txt";

Engine engine(80, 50);

int main() {
	// TODO this is so nasty I need to wash my hands
	// show menu on startup

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
	while(!TCODConsole::isWindowClosed()) {
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
