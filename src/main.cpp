#include "main.hpp"

std::string file = "save.txt";

Engine engine(80, 50);

int main() {
	if(TCODSystem::fileExists("save.txt")) {
		std::ifstream ifs(file);
		boost::archive::text_iarchive ia(ifs);
		ia >> engine;
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
	// Save starts
	std::ofstream ofs(file);
	boost::archive::text_oarchive oa(ofs);
	oa << engine;   
	// Save ends

	return 0;
}
