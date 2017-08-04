#include <fstream>
#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>

#include "persistent.hpp"

#include "libtcod.hpp"
#include "engine.hpp"
#include "main.hpp"

std::string file = "save.txt";

Engine engine(1);

int main() {
	showMenu();

	while(!TCODConsole::isWindowClosed()) {
		TCOD_key_t lastKey;
		TCOD_mouse_t mouse;
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE, &lastKey, &mouse);
		engine.mouse = mouse;
		if(lastKey.vk == TCODK_ESCAPE) {
			save();
			load();
			showMenu();
		}
		engine.update();
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

	engine.render();
	TCODConsole::flush();
}
