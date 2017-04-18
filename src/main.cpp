#include "main.hpp"

std::string file = "save.txt";

Engine engine(80, 50);

int main() {
	if(TCODSystem::fileExists("savegame.zip")) {
		// TODO implement loading
	}
	else {
		engine.init();
	}
	//engine.load();
	while(!TCODConsole::isWindowClosed()) {
		engine.update();
		engine.render();
		TCODConsole::flush();
	}
	//engine.save();

	// TODO implement permadeath
	std::ofstream ofs(file);
	boost::archive::text_oarchive oa(ofs);
	oa << engine;   

	return 0;
}
